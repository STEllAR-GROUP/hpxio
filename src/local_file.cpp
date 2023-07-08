//  Copyright (c) 2015 Alireza Kheirkhahan
//  Copyright (c) 2014 Shuangyang Yang
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/config.hpp>
#if !defined(HPX_COMPUTE_DEVICE_CODE)
#include <hpx/include/components.hpp>
#include <hpx/include/serialization.hpp>

#include <hpxio/server/local_file.hpp>

///////////////////////////////////////////////////////////////////////////////
// Add factory registration functionality
HPX_REGISTER_COMPONENT_MODULE()

///////////////////////////////////////////////////////////////////////////////

typedef hpx::components::component<hpx::io::server::local_file> local_file_type;

///////////////////////////////////////////////////////////////////////////////
/// Serialization support for the local_file actions
HPX_REGISTER_ACTION(
    local_file_type::wrapped_type::open_action,
    local_file_open_action)
HPX_REGISTER_ACTION(
    local_file_type::wrapped_type::is_open_action,
    local_file_is_open_action)
HPX_REGISTER_ACTION(
    local_file_type::wrapped_type::close_action,
    local_file_close_action)
HPX_REGISTER_ACTION(
    local_file_type::wrapped_type::remove_file_action,
    local_file_remove_file_action)
HPX_REGISTER_ACTION(
    local_file_type::wrapped_type::read_action,
    local_file_read_action)
HPX_REGISTER_ACTION(
    local_file_type::wrapped_type::pread_action,
    local_file_pread_action)
HPX_REGISTER_ACTION(
    local_file_type::wrapped_type::write_action,
    local_file_write_action)
HPX_REGISTER_ACTION(
    local_file_type::wrapped_type::pwrite_action,
    local_file_pwrite_action)
HPX_REGISTER_ACTION(
    local_file_type::wrapped_type::lseek_action,
    local_file_lseek_action)
HPX_REGISTER_ACTION(
    local_file_type::wrapped_type::tell_action,
    local_file_tell_action)

HPX_REGISTER_COMPONENT(local_file_type, local_file)

/// I dont know what this does.
HPX_REGISTER_ACTION(hpx::lcos::base_lco_with_value<
                            std::vector<std::vector<double>>>::set_value_action,
                    set_value_action_vector_vector_double)
HPX_DEFINE_GET_COMPONENT_TYPE_STATIC(
        hpx::lcos::base_lco_with_value<std::vector<std::vector<double>>>,
        hpx::components::component_base_lco_with_value);

HPX_REGISTER_ACTION(
        hpx::lcos::base_lco_with_value<std::vector<double>>::set_value_action,
        set_value_action_vector_double)
HPX_DEFINE_GET_COMPONENT_TYPE_STATIC(
        hpx::lcos::base_lco_with_value<std::vector<double>>,
        hpx::components::component_base_lco_with_value);

#endif