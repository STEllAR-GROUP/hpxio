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
                                     was_created_(false) {

    }

    io_dispatcher::io_dispatcher(hpx::future<hpx::id_type> &&id) :
            base_type(std::move(id)),
            num_partitions_(0),
            bytes_per_partition_(0),
            was_created_(false) {

    }

//     TODO : Can open a file separately. How to handle byte allocation when writing or appending?
    io_dispatcher::io_dispatcher(std::string const& file_name, std::string const& mode,
                                 std::string const& symbolic_name_base,
                                 std::size_t num_instances)
            : base_type(hpx::new_<config_data_type>(hpx::find_here(),
                                                    config_data(file_name, symbolic_name_base, num_instances))),
              num_partitions_(num_instances){
        hpx::future<void> f = register_as(symbolic_name_base);

        // TODO: add error handling
        // initialise everything
        initialise(file_name, mode, symbolic_name_base, num_instances);

        f.get();
        was_created_ = true;
    }

    void io_dispatcher::initialise(std::string const& file_name, std::string const& mode,
                    std::string symbolic_base_name, std::size_t num_instances) {

        //get the file size, need uintmax_t for large files
        std::uintmax_t file_size = hpx::filesystem::file_size(file_name);
        this->bytes_per_partition_ = (file_size + num_instances - 1) / num_instances;
        this->mode_ = mode;

        //create the partitions
        std::vector<hpx::id_type> localities = hpx::find_all_localities();
        hpx::future<std::vector<hpx::io::local_file>> result =
                hpx::new_<hpx::io::local_file[]>(hpx::default_layout(localities), num_instances);

        // Do I need to get ?
        partitions_ = result.get();
        std::vector<hpx::future<void>> lazy_sync;
        for (auto partition: partitions_) {
           lazy_sync.push_back(partition.open(file_name, mode));
        }

        if (symbolic_base_name.back() != '/') {
            symbolic_base_name.push_back('/');
        }

        std::size_t counter = 0;
        for (hpx::io::local_file& partition: partitions_) {
            lazy_sync.push_back(partition.register_as(symbolic_base_name
            + std::to_string(counter++)));
        }

        FILE* file_ = fopen(file_name.c_str(), mode.c_str());
        this->pointer = ftell(file_);
        fclose(file_);

        hpx::wait_all(lazy_sync);
    }

    io_dispatcher::~io_dispatcher() {
        if (was_created_) {
            // close all  local_file
            for (auto partition: partitions_) {
                partition.close().get();
            }

            // unregister base name
            typedef config_data_type::get_action act;
            config_data data = act()(get_id());
            std::string sym_name = data.symbolic_name_;

            hpx::agas::unregister_name(hpx::launch::sync, sym_name);

            if (sym_name.back() != '/') {
                sym_name.push_back('/');
            }

            for (std::size_t i = 0; i < num_partitions_; ++i) {
                hpx::agas::unregister_name(hpx::launch::sync, sym_name + std::to_string(i));
            }
        }
    }

    /// I/O functions

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

    std::vector<char> io_dispatcher::read_at(std::uintmax_t offset, std::size_t size)
    {
//        HPX_ASSERT(this->mode_ == "r" || this->mode_ == "r+" || this->mode_ == "w+" || this->mode_ == "a+");
        int start = offset / bytes_per_partition_;
        int end = (offset + size) / bytes_per_partition_;
        std::vector<hpx::future<std::vector<char>>> lazy_reads;
        std::vector<char> result;

        // TODO : Can ue pread instead of lseek + read

        std::vector<hpx::future<int>> lazy_seek;
        lazy_seek.push_back(partitions_[start].lseek(offset, SEEK_SET));
        for (int i = start + 1; i <= end; ++i) {
            lazy_seek.push_back(partitions_[i].lseek(i * bytes_per_partition_, SEEK_SET));
        }

        for (int i = start; i <= end; ++i) {
            lazy_seek[i - start].get();
            lazy_reads.push_back(partitions_[i].read((i + 1) * bytes_per_partition_ - std::max(offset, i * bytes_per_partition_)));
        }

        for (auto& read: lazy_reads) {
            std::vector<char> tmp = read.get();
            result.insert(result.end(), tmp.begin(), tmp.end());
        }

        return result;
    }

    hpx::future<std::vector<char> > io_dispatcher::read_at_async(std::uintmax_t offset, std::size_t size) {
        return hpx::async(hpx::bind(&io_dispatcher::read_at, this, offset, size));
    }

    void io_dispatcher::write_at(std::uintmax_t offset, std::vector<char> data) {
//        HPX_ASSERT(this->mode_ == "w" || this->mode_ == "w+" || this->mode_ == "r+" || this->mode_ == "a+");
        int start = offset / bytes_per_partition_;
        int end = (offset + data.size()) / bytes_per_partition_;
        std::vector<hpx::future<void>> lazy_writes;

        // TODO : Can ue pwrite instead of lseek + write

        std::vector<hpx::future<int>> lazy_seek;
        lazy_seek.push_back(partitions_[start].lseek(offset, SEEK_SET));
        for (int i = start + 1; i <= end; ++i) {
            lazy_seek.push_back(partitions_[i].lseek(i * bytes_per_partition_, SEEK_SET));
        }

//        Do writes need to be synchronous?
        for (int i = start; i <= end; ++i) {
            lazy_seek[i - start].get();
            std::vector<char> tmp(data.begin() + std::max(offset, i * bytes_per_partition_), data.begin() + std::min((i + 1) * bytes_per_partition_, offset + data.size()));
            lazy_writes.push_back(partitions_[i].write(tmp));
        }

        hpx::wait_all(lazy_writes);
    }

    hpx::future<void> io_dispatcher::write_at_async(std::uintmax_t offset, std::vector<char> data) {
        return hpx::async(hpx::bind(&io_dispatcher::write_at, this, offset, data));
    }
}
