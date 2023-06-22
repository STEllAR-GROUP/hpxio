//  Copyright (c) 2007-2017 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/hpx.hpp>

#include <cstddef>
#include <string>

namespace hpx::io {
    ///////////////////////////////////////////////////////////////////////////
    struct config_data
    {
        config_data()
          : num_instances_(0)
        {
        }

        config_data(std::string const& datafile_name, std::string const& mode,
            std::string const& symbolic_name, std::size_t num_instances)
          : datafile_name_(datafile_name)
          , mode_(mode)
          , symbolic_name_(symbolic_name)
          , num_instances_(num_instances)
        {
        }

        std::string datafile_name_;    // Data file to load the data from.
        std::string mode_;              // Mode to open the data file in.
        std::string symbolic_name_;    // Symbolic name this instance is 
                                       // registered with.
        std::size_t num_instances_;    // Number of partition instances.
    };
}    // namespace sheneos

HPX_DISTRIBUTED_METADATA_DECLARATION(hpx::io::config_data, hpxio_config_data)

///////////////////////////////////////////////////////////////////////////////
// Non-intrusive serialization.
namespace hpx { namespace serialization {
    HPX_COMPONENT_EXPORT void serialize(
        input_archive& ar, hpx::io::config_data& cfg, unsigned int const);

    HPX_COMPONENT_EXPORT void serialize(
        output_archive& ar, hpx::io::config_data& cfg, unsigned int const);
}}    // namespace hpx::serialization
