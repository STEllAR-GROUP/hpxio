#include <hpx/hpx.hpp>

#include <hpx/assert.hpp>
#include <hpx/include/async.hpp>
#include <hpx/include/lcos_local.hpp>
#include <hpx/include/runtime.hpp>

#include "../hpxio/io_dispatcher.hpp"
#include "../hpxio/local_file.hpp"

HPX_REGISTER_COMPONENT_MODULE()

namespace hpx::io {
    io_dispatcher::io_dispatcher() : num_partitions_(0),
                                     bytes_per_partition_(0),
                                     file_size_(0),
                                     was_created_(false) {
        file_name_.clear();
        mode_.clear();
    }

    io_dispatcher::io_dispatcher(hpx::future<hpx::id_type> &&id) :
            base_type(std::move(id)),
            num_partitions_(0),
            bytes_per_partition_(0),
            file_size_(0),
            was_created_(false) {
        file_name_.clear();
        mode_.clear();
    }

//     TODO : Can open a file separately. How to handle byte allocation when writing or appending?
    io_dispatcher::io_dispatcher(std::string const &file_name, std::string const &mode,
            std::string const& symbolic_name_base, std::size_t num_instances)
            : base_type(hpx::new_<config_data_type>(hpx::find_here(),
                                                    config_data(symbolic_name_base, num_instances))),
              num_partitions_(num_instances),
              was_created_(false),
              file_size_(0) {
        register_as(symbolic_name_base).get();
        file_name_.clear();
        mode_.clear();

        // initialise everything
        initialise(symbolic_name_base, num_instances);

        // open the file and set the mode on all partitions
        std::vector<hpx::future<void>> lazy_open;
        for (auto partition: partitions_) {
            lazy_open.push_back(partition.open(file_name, mode));
        }

        //get the file size, need off_t for large files
        this->file_name_ = file_name;
        file_size_ = hpx::filesystem::file_size(file_name);
        this->bytes_per_partition_ = (file_size_ + num_partitions_ - 1) / num_partitions_;
        this->mode_ = mode;

        hpx::wait_all(lazy_open);

        this->pointer = partitions_[0].tell().get();
        was_created_ = true;
    }

    void io_dispatcher::initialise(std::string symbolic_base_name, std::size_t num_instances) {
        //create the partitions
        std::vector<hpx::id_type> localities = hpx::find_all_localities();
        hpx::future<std::vector<hpx::io::local_file>> result =
                hpx::new_<hpx::io::local_file[]>(hpx::default_layout(localities), num_instances);

        // Do I need to get ?
        partitions_ = result.get();
        std::vector<hpx::future<void>> lazy_sync;

        if (symbolic_base_name[symbolic_base_name.size() - 1] != '/') {
            symbolic_base_name += '/';
        }

        std::size_t counter = 0;
        for (hpx::io::local_file &partition: partitions_) {
            lazy_sync.push_back(partition.register_as(symbolic_base_name
                                                      + std::to_string(counter++)));
        }

        hpx::wait_all(lazy_sync);
    }

    io_dispatcher::~io_dispatcher() {
        hpx::future<void> f = partitions_[0].close();
        if (was_created_) {
            // unregister base name
            typedef config_data_type::get_action act;
            config_data data = act()(get_id());

            hpx::agas::unregister_name(hpx::launch::sync, data.symbolic_name_);

            if (data.symbolic_name_[data.symbolic_name_.size() - 1] != '/') {
               data.symbolic_name_ += '/';
            }

            for (std::size_t i = 1; i <= num_partitions_; ++i) {
                hpx::agas::unregister_name(hpx::launch::sync, data.symbolic_name_ + std::to_string(i));
            }
        }
        f.get();
    }

    /// I/O functions

    void io_dispatcher::open(std::string const &file_name, std::string const &mode) {
        // close already oepn file
        close();

        std::vector<hpx::future<void>> lazy_open;
        for (auto partition: partitions_) {
            lazy_open.push_back(partition.open(file_name, mode));
        }

        hpx::wait_all(lazy_open);

        this->pointer = partitions_[0].tell().get();

        //get the file size, need off_t for large files
        this->file_name_ = file_name;
        file_size_ = hpx::filesystem::file_size(file_name);
        this->bytes_per_partition_ = (file_size_ + num_partitions_ - 1) / num_partitions_;
        this->mode_ = mode;
    }
    /// TODO : Remove open and close functions. They are not needed.
    void io_dispatcher::close() {
        if (file_name_.empty()) {
            return;
        }

        std::vector<hpx::future<void>> lazy_close;
        for (auto partition: partitions_) {
            lazy_close.push_back(partition.close());
        }

        hpx::wait_all(lazy_close);
        file_name_.clear();
        mode_.clear();
    }

    std::vector<char> io_dispatcher::read(std::size_t size) {
        std::vector<char> result = read_at(pointer, size);
        pointer += size;
        return result;
    }

    hpx::future<std::vector<char>> io_dispatcher::read_async(std::size_t size) {
        hpx::future<std::vector<char>> result = read_at_async(pointer, size);
        pointer += size;
        return result;
    }

    std::vector<char> io_dispatcher::read_at(off_t offset, std::size_t size) {
//        HPX_ASSERT(this->mode_ == "r" || this->mode_ == "r+" || this->mode_ == "w+" || this->mode_ == "a+");
        int start = offset / bytes_per_partition_;
        int end;
        if (bytes_per_partition_)
            end = std::min(static_cast<size_t>((offset + size) / bytes_per_partition_), num_partitions_ - 1);
        else {
            return partitions_[0].pread(size, offset).get();
        }
        std::vector<hpx::future<std::vector<char>>> lazy_reads;
        std::vector<char> result;

        for (int i = start; i <= end; i++) {
            off_t off_i = std::max(offset, i * bytes_per_partition_);
            lazy_reads.push_back(partitions_[i].pread((i + 1) * bytes_per_partition_ - off_i, off_i));
        }

        for (auto &read: lazy_reads) {
            std::vector<char> tmp = read.get();
            result.insert(result.end(), tmp.begin(), tmp.end());
        }

        return result;
    }

    hpx::future<std::vector<char> > io_dispatcher::read_at_async(off_t offset, std::size_t size) {
        return hpx::async(hpx::bind(&io_dispatcher::read_at, this, offset, size));
    }

/// TODO : Implement lazy writes. Look into parallel writes?

    ssize_t io_dispatcher::write(std::vector<char> data) {
        ssize_t result = write_at_async(pointer, data).get();
        pointer += result;
        return result;
    }

    hpx::future<ssize_t> io_dispatcher::write_async(std::vector<char> data) {
        return hpx::async(hpx::bind(&io_dispatcher::write, this, data));
    }

    ssize_t io_dispatcher::write_at(off_t offset, std::vector<char> data) {
        return write_at_async(offset, data).get();
    }

    hpx::future<ssize_t> io_dispatcher::write_at_async(off_t offset, std::vector<char> data) {
        return partitions_[0].pwrite(data, offset);
    }

    void io_dispatcher::seek(off_t offset, int whence) {
        switch (whence) {
            case SEEK_SET:
                pointer = offset;
                break;
            case SEEK_CUR:
                pointer += offset;
                break;
            case SEEK_END:
                pointer = file_size_ + offset;
                break;
            default:
                throw std::runtime_error("Invalid whence argument");
        }
    }

    off_t io_dispatcher::tell() {
        return pointer;
    }
}
