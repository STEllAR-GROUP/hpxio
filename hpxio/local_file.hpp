//  Copyright (c) 2015 Alireza Kheirkhahan
//  Copyright (c) 2014 Shuangyang Yang
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(HPX_COMPONENTS_IO_LOCAL_FILE_HPP_AUG_26_2014_1102AM)
#define HPX_COMPONENTS_IO_LOCAL_FILE_HPP_AUG_26_2014_1102AM

//#include <hpx/include/client.hpp>
#include <hpx/include/components.hpp>
#include <hpxio/server/local_file.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace io
{

    ///////////////////////////////////////////////////////////////////////////
    /// The \a local_file class is the client side representation of a
    /// concrete \a server#local_file component
    class local_file :
public hpx::components::client_base<local_file, server::local_file>
    {
    private:
        typedef components::client_base<local_file, server::local_file>
            base_type;

    public:
        local_file() {}

        local_file(hpx::id_type&& gid) : base_type(std::move(gid)) {}

        local_file(hpx::future<hpx::id_type> && gid)
          : base_type(std::move(gid))
        {}

        hpx::future<void> open(std::string const& name, std::string const& mode)
        {
            typedef server::local_file::open_action action_type;
            return hpx::async<action_type>(this->get_id(),
                    name, mode);
        }

        void open_sync(std::string const& name, std::string const& mode)
        {
            return open(name, mode).get();
        }

        hpx::future<bool> is_open()
        {
            typedef server::local_file::is_open_action action_type;
            return hpx::async<action_type>(this->get_id());
        }

        bool is_open_sync()
        {
            return is_open().get();
        }

        hpx::future<void> close()
        {
            typedef server::local_file::close_action action_type;
            return hpx::async<action_type>(this->get_id());
        }

        void close_sync()
        {
            return close().get();
        }

        hpx::future<int> remove_file(std::string const& file_name)
        {
            typedef server::local_file::remove_file_action action_type;
            return hpx::async<action_type>(this->get_id(),
                    file_name);
        }

        int remove_file_sync(std::string const& file_name)
        {
            return remove_file(file_name).get();
        }

        hpx::future<std::vector<char> > read(size_t const& count)
        {
            typedef server::local_file::read_action action_type;
            return hpx::async<action_type>(this->get_id(),
                    count);
        }

        std::vector<char> read_sync(size_t const count)
        {
            return read(count).get();
        }

        hpx::future<std::vector<char> > pread(ssize_t const count,
                off_t const offset)
        {
            typedef server::local_file::pread_action action_type;
            return hpx::async<action_type>(this->get_id(),
                    count, offset);
        }

        std::vector<char> pread_sync(size_t const count, off_t const offset)
        {
            return pread(count, offset).get();
        }

        hpx::future<ssize_t> write(std::vector<char> const& buf)
        {
            typedef server::local_file::write_action action_type;
            return hpx::async<action_type>(this->get_id(),
                    buf);
        }

        ssize_t write_sync(std::vector<char> const& buf)
        {
            return write(buf).get();
        }

        hpx::future<ssize_t> pwrite(std::vector<char> const& buf,
                off_t const offset)
        {
            typedef server::local_file::pwrite_action action_type;
            return hpx::async<action_type>(this->get_id(),
                    buf, offset);
        }

        ssize_t pwrite_sync(std::vector<char> const& buf, off_t const offset)
        {
            return pwrite(buf, offset).get();
        }

        hpx::future<int> lseek(off_t const offset, int const whence)
        {
            typedef server::local_file::lseek_action action_type;
            return hpx::async<action_type>(this->get_id(),
                    offset, whence);
        }

        int lseek_sync(off_t const offset, int const whence)
        {
            return lseek(offset, whence).get();
        }

    };

}} // hpx::io

#endif

