// copyrights ??

#pragma once

#include <hpx/hpx.hpp>

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "local_file.hpp"
#include "server/configuration.hpp"

namespace hpx::io {
    class HPX_COMPONENT_EXPORT io_dispatcher
            : public hpx::components::client_base<io_dispatcher,
                    hpx::components::server::distributed_metadata_base<
                            hpx::io::config_data>> {
        /// typedef component names
    private:
        typedef hpx::components::server::distributed_metadata_base<
                hpx::io::config_data> config_data_type;

        typedef hpx::components::client_base<io_dispatcher, config_data_type>
                base_type;

        /// data members
    private:
        std::vector<hpx::io::local_file> partitions_;
        std::size_t num_partitions_;
        off_t file_size_;
        off_t bytes_per_partition_;
        std::string file_name_, mode_;
        bool was_created_;
        off_t pointer;

    private:

        void initialise(std::string symbolic_base_name, std::size_t num_instances = std::size_t(-1));

    public:
        /// constructors
        io_dispatcher();

        io_dispatcher(std::string const &file_name, std::string const &mode,
                               std::string const& symbolic_name_base = "/hpxio/io_dispatcher",
                               std::size_t num_instances = std::size_t(1));

        explicit io_dispatcher(hpx::future<hpx::id_type> &&id);

        /// destructor
        ~io_dispatcher();

        /// Connect to an existing interpolation object with the given symbolic name
//        void connect(std::string symbolic_name_base = "/hpxio/io_dispatcher");

        std::vector<char> read(std::size_t size);

        hpx::future<std::vector<char> > read_async(std::size_t size);

        std::vector<char> read_at_work(off_t offset, std::size_t size);

        std::vector<char> read_at(off_t offset, std::size_t size);
        hpx::future<std::vector<char> > read_at_async(off_t offset, std::size_t size);

        ssize_t write(std::vector<char> data);

        hpx::future<ssize_t> write_async(std::vector<char> data);

        ssize_t write_at(off_t offset, std::vector<char> data);

        hpx::future<ssize_t> write_at_async(off_t offset, std::vector<char> data);

        void seek(off_t offset, int whence);

        off_t tell();
    };
}