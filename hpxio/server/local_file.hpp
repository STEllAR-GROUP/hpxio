//  Copyright (c) 2015 Alireza Kheirkhahan
//  Copyright (c) 2014 Shuangyang Yang
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <hpx/config.hpp>

#if !defined(HPX_COMPUTE_DEVICE_CODE)

#include <hpx/hpx.hpp>
#include <hpx/runtime_local/service_executors.hpp>

#include <hpx/execution.hpp>
#include <hpx/functional/bind.hpp>

#include <cstdio>
#include <vector>
#include <string>

#if defined(BOOST_MSVC)
#ifdef _WIN64
typedef __int64 ssize_t;
#else
typedef _W64 int ssize_t;
#endif
#endif

///////////////////////////////////////////////////////////////////////////////
namespace hpx::io::server {
    // local file class
    // uses C style file APIs
    class HPX_COMPONENT_EXPORT local_file
            : public hpx::components::locking_hook<
                    hpx::components::component_base<local_file>> {
    public:
        local_file();

        ~local_file();

        void open(std::string const &name, std::string const &mode);


        bool is_open() const;

        void close();

        int remove_file(std::string const &file_name);

        std::vector<char> read(size_t count);

        std::vector<char> pread(size_t count, off_t offset);

        ssize_t write(std::vector<char> const &buf);

        ssize_t pwrite(std::vector<char> const &buf,
                       off_t offset);

        int lseek(off_t offset, int whence);

        ///////////////////////////////////////////////////////////////////////
        /// Each of the exposed functions needs to be encapsulated into a action
        /// type, allowing to generate all require boilerplate code for threads,
        /// serialization, etc.
        HPX_DEFINE_COMPONENT_ACTION(local_file, open);
        HPX_DEFINE_COMPONENT_ACTION(local_file, is_open);
        HPX_DEFINE_COMPONENT_ACTION(local_file, close);
        HPX_DEFINE_COMPONENT_ACTION(local_file, remove_file);
        HPX_DEFINE_COMPONENT_ACTION(local_file, read);
        HPX_DEFINE_COMPONENT_ACTION(local_file, pread);
        HPX_DEFINE_COMPONENT_ACTION(local_file, write);
        HPX_DEFINE_COMPONENT_ACTION(local_file, pwrite);
        HPX_DEFINE_COMPONENT_ACTION(local_file, lseek);

    private:
        void open_work(std::string const &name, std::string const &mode);

        void close_work();

        void remove_file_work(std::string const &file_name, int &result);

        void read_work(size_t count, std::vector<char> &result);

        void pread_work(size_t count, off_t offset,
                        std::vector<char> &result);

        void write_work(std::vector<char> const &buf,
                        ssize_t &result);

        void pwrite_work(std::vector<char> const &buf,
                         off_t offset, ssize_t &result);

        void lseek_work(off_t offset, int whence, int &result);

    private:
        typedef hpx::components::managed_component_base<local_file> base_type;

        std::FILE *fp_;
        std::string file_name_;
    };

} // hpx::io::server

///////////////////////////////////////////////////////////////////////////////
// Declaration of serialization support for the local_file actions
HPX_REGISTER_ACTION_DECLARATION(hpx::io::server::local_file::open_action,
                                local_file_open_action)
HPX_REGISTER_ACTION_DECLARATION(hpx::io::server::local_file::is_open_action,
                                local_file_is_open_action)
HPX_REGISTER_ACTION_DECLARATION(hpx::io::server::local_file::close_action,
                                local_file_close_action)
HPX_REGISTER_ACTION_DECLARATION(hpx::io::server::local_file::remove_file_action,
                                local_file_remove_file_action)
HPX_REGISTER_ACTION_DECLARATION(hpx::io::server::local_file::read_action,
                                local_file_read_action)
HPX_REGISTER_ACTION_DECLARATION(hpx::io::server::local_file::pread_action,
                                local_file_pread_action)
HPX_REGISTER_ACTION_DECLARATION(hpx::io::server::local_file::write_action,
                                local_file_write_action)
HPX_REGISTER_ACTION_DECLARATION(hpx::io::server::local_file::pwrite_action,
                                local_file_pwrite_action)
HPX_REGISTER_ACTION_DECLARATION(hpx::io::server::local_file::lseek_action,
                                local_file_lseek_action)

/// What do these do? Might be needed for linking
HPX_REGISTER_ACTION_DECLARATION(hpx::lcos::base_lco_with_value<
        std::vector<std::vector<double>>>::set_value_action,
        set_value_action_vector_vector_double)

HPX_REGISTER_ACTION_DECLARATION(hpx::lcos::base_lco_with_value<
        std::vector<double>>::set_value_action,
        set_value_action_vector_double)

#endif
