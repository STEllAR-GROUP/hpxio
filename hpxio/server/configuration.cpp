#include <hpx/serialization.hpp>

#include <cstddef>
#include <string>

#include "configuration.hpp"

namespace hpx { namespace serialization {
    void serialize(
        input_archive& ar, hpx::io::config_data& cfg, unsigned int const)
    {
        ar& cfg.datafile_name_& cfg.symbolic_name_& cfg.num_instances_;
    }

    void serialize(
        output_archive& ar, hpx::io::config_data& cfg, unsigned int const)
    {
        ar& cfg.datafile_name_& cfg.symbolic_name_& cfg.num_instances_;
    }
}}

HPX_DISTRIBUTED_METADATA(hpx::io::config_data, hpx_io_config_data)