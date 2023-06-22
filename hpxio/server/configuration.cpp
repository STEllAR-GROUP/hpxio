#include <hpx/serialization.hpp>

#include <cstddef>
#include <string>

#include "configuration.hpp"

namespace hpx { namespace serialization {
    void serialize(
        input_archive& ar, hpx::io::config_data& cfg, unsigned int const)
    {
        ar& cfg.file_name& cfg.symbolic_name_& cfg.num_instances_;
    }

    void serialize(
        output_archive& ar, hpx::io::config_data& cfg, unsigned int const)
    {
        ar& cfg.file_name& cfg.symbolic_name_& cfg.num_instances_;
    }
}}