//
// Created by arnav on 25/6/23.
//

#include <hpx/config.hpp>
#if !defined(HPX_COMPUTE_DEVICE_CODE)
#include <hpx/hpx.hpp>
#include <hpx/runtime_local/service_executors.hpp>
#include <hpx/execution.hpp>
#include <hpx/functional/bind.hpp>

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include "local_file.hpp"

///////////////////////////////////////////////////////////////////////////////
namespace hpx::io::server {
    local_file::local_file() {
        fp_ = NULL;
        file_name_.clear();
    }

    local_file::~local_file() {
        close();
    }

    void local_file::open(std::string const &name, std::string const &mode)
    {
        hpx::parallel::execution::io_pool_executor exec;
        hpx::parallel::execution::post(exec, hpx::bind(&local_file::open_work, this, name, mode));
    }

    void local_file::open_work(std::string const &name, std::string const &mode)
    {
        if (fp_ != NULL)
        {
            close();
        }
        fp_ = fopen(name.c_str(), mode.c_str());
        file_name_ = name;
    }

    bool local_file::is_open() const
    {
        return fp_ != NULL;
    }

    void local_file::close()
    {
        hpx::parallel::execution::io_pool_executor exec;
        hpx::parallel::execution::post(exec, hpx::bind(&local_file::close_work, this));
    }

    void local_file::close_work()
    {
        if (fp_ != NULL)
        {
            std::fclose(fp_);
            fp_ = NULL;
        }
        file_name_.clear();
    }

    int local_file::remove_file(std::string const &file_name)
    {
        int result;
        {
            hpx::parallel::execution::io_pool_executor exec;
            hpx::parallel::execution::post(exec, hpx::bind(&local_file::remove_file_work, this, file_name, std::ref(result)));
        }
        return result;
    }

    void local_file::remove_file_work(std::string const &file_name, int &result)
    {
        result = std::remove(file_name.c_str());
    }

    std::vector<char> local_file::read(size_t const count)
    {
        std::vector<char> result;
        {
            hpx::parallel::execution::io_pool_executor exec;
            hpx::parallel::execution::async_execute(exec, hpx::bind(&local_file::read_work, this, count, std::ref(result))).get();
        }
        return result;
    }

    void local_file::read_work(size_t const count, std::vector<char> &result)
    {
        if (fp_ == NULL || count <= 0)
        {
            return;
        }

        std::unique_ptr<char> sp(new char[count]);
        ssize_t len = fread(sp.get(), 1, count, fp_);

        if (len == 0)
        {
            return;
        }

        result.assign(sp.get(), sp.get() + len);
    }

    std::vector<char> local_file::pread(size_t const count, off_t const offset)
    {
        std::vector<char> result;
        {
            hpx::parallel::execution::io_pool_executor exec;
            hpx::parallel::execution::async_execute(exec, hpx::bind(&local_file::pread_work, this, count, offset, std::ref(result))).get();
        }
        return result;
    }

    void local_file::pread_work(size_t const count, off_t const offset,
                    std::vector<char> &result)
    {
        if (fp_ == NULL || count <= 0 || offset < 0)
        {
            return;
        }

        fpos_t pos;
        if (fgetpos(fp_, &pos) != 0)
        {
            return;
        }

        if (fseek(fp_, offset, SEEK_SET) != 0)
        {
            fsetpos(fp_, &pos);
            return;
        }

        read_work(count, result);
        fsetpos(fp_, &pos);
    }

    ssize_t local_file::write(std::vector<char> const &buf)
    {
        ssize_t result = 0;
        {
            hpx::parallel::execution::io_pool_executor exec;
            hpx::parallel::execution::async_execute(exec, hpx::bind(&local_file::write_work, this, buf, std::ref(result))).get();
        }
        return result;
    }

    void local_file::write_work(std::vector<char> const &buf,
                    ssize_t &result)
    {
        if (fp_ == NULL || buf.

                empty()

                )
        {
            return;
        }
        result = fwrite(buf.data(), 1, buf.size(), fp_);
    }

    ssize_t local_file::pwrite(std::vector<char> const &buf,
                   off_t const offset)
    {
        ssize_t result = 0;
        {
            hpx::parallel::execution::io_pool_executor exec;
            hpx::parallel::execution::async_execute(exec, hpx::bind(&local_file::pwrite_work, this, buf, offset, std::ref(result))).get();
        }
        return result;
    }

    void local_file::pwrite_work(std::vector<char> const &buf,
                     off_t const offset, ssize_t &result)
    {
        if (fp_ == NULL || buf.

                empty()

            || offset < 0)
        {
            return;
        }

        fpos_t pos;
        if (
                fgetpos(fp_, &pos) != 0)
        {
            return;
        }

        if (
                fseek(fp_, offset, SEEK_SET) != 0)
        {
            fsetpos(fp_, &pos);
            return;
        }

        write_work(buf, result);
        fsetpos(fp_, &pos);
    }

    int local_file::lseek(off_t const offset, int const whence)
    {
        int result;
        {
            hpx::parallel::execution::io_pool_executor exec;
            hpx::parallel::execution::async_execute(exec, hpx::bind(&local_file::lseek_work, this, offset, whence, std::ref(result))).get();
        }
        return result;
    }

    void local_file::lseek_work(off_t const offset, int const whence, int &result)
    {
        if (fp_ == NULL)
        {
            result = -1;
            return;
        }

        result = fseek(fp_, offset, whence);
    }
}

///////////////////////////////////////////////////////////////////////////////
/// serialization support for the local_file actions
HPX_REGISTER_ACTION(hpx::io::server::local_file::open_action,
                                local_file_open_action)
HPX_REGISTER_ACTION(hpx::io::server::local_file::is_open_action,
                                local_file_is_open_action)
HPX_REGISTER_ACTION(hpx::io::server::local_file::close_action,
                                local_file_close_action)
HPX_REGISTER_ACTION(hpx::io::server::local_file::remove_file_action,
                                local_file_remove_file_action)
HPX_REGISTER_ACTION(hpx::io::server::local_file::read_action,
                                local_file_read_action)
HPX_REGISTER_ACTION(hpx::io::server::local_file::pread_action,
                                local_file_pread_action)
HPX_REGISTER_ACTION(hpx::io::server::local_file::write_action,
                                local_file_write_action)
HPX_REGISTER_ACTION(hpx::io::server::local_file::pwrite_action,
                                local_file_pwrite_action)
HPX_REGISTER_ACTION(hpx::io::server::local_file::lseek_action,
                                local_file_lseek_action)

HPX_REGISTER_COMPONENT(hpx::components::component<
                        hpx::io::server::local_file>,
                        local_file);

/// What do these do? Might be needed for linking
HPX_REGISTER_ACTION(hpx::lcos::base_lco_with_value<
                                        std::vector<std::vector<double>>>::set_value_action,
                                set_value_action_vector_vector_double)
HPX_DEFINE_GET_COMPONENT_TYPE_STATIC(
        hpx::lcos::base_lco_with_value<std::vector<std::vector<double>>>,
        hpx::components::component_base_lco_with_value);


HPX_REGISTER_ACTION(hpx::lcos::base_lco_with_value<
                                        std::vector<double>>::set_value_action,
                                set_value_action_vector_double)
HPX_DEFINE_GET_COMPONENT_TYPE_STATIC(
        hpx::lcos::base_lco_with_value<std::vector<double>>,
        hpx::components::component_base_lco_with_value);
#endif

