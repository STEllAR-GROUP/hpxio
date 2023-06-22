#include <hpx/hpx.hpp>

#include <hpx/assert.hpp>
#include <hpx/include/async.hpp>
#include <hpx/include/lcos_local.hpp>
#include <hpx/include/runtime.hpp>

#include <hpxio/io_dispatcher.hpp>

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

    io_dispatcher::io_dispatcher(std::string const& file_name, std::string const& mode,
                                 std::string const& symbolic_name_base,
                                 std::size_t num_instances)
            : base_type(hpx::new_<config_data_type>(hpx::find_here(),
                                                    config_data(file_name, mode,
                                                                symbolic_name_base, num_instances))),
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
        bytes_per_partition_ = (file_size + num_instances - 1) / num_instances;

        //create the partitions
        std::vector<hpx::id_type> localities = hpx::find_all_localities();
        hpx::future<std::vector<hpx::io::local_file>> result =
                hpx::new_<local_file[]>(hpx::default_layout(localities), num_instances);

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
        for (local_file& partition: partitions_) {
            lazy_sync.push_back(partition.register_as(symbolic_base_name
            + std::to_string(counter++)));
        }

        hpx::wait_all(lazy_sync);
    }

    io_dispatcher::~io_dispatcher() {
        if (was_created_) {
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
}
