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
        std::uintmax_t bytes_per_partition_;
        std::string mode_;
        bool was_created_;

    private:

        void initialise(std::string const& file_name, std::string const& mode,
                        std::string symbolic_base_name, std::size_t num_instances = std::size_t(-1));

    public:
        /// constructors
        io_dispatcher();

        io_dispatcher(std::string const& file_name, std::string const& mode,
                std::string const& symbolic_name_base = "/hpxio/io_dispatcher",
                std::size_t num_instances = std::size_t(-1));

        explicit io_dispatcher(hpx::future<hpx::id_type>&& id);

        /// destructor
        ~io_dispatcher();

        /// Connect to an existing interpolation object with the given symbolic name
//        void connect(std::string symbolic_name_base = "/hpxio/io_dispatcher");

        /// TODO: add all functions for file io
//        std::vector<char> read(std::uintmax_t offset, std::size_t size);
//
//        void write(std::uintmax_t offset, std::vector<char> data);
    };
}