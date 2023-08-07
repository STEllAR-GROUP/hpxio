//  Copyright (c) 2015 Alireza Kheirkhahan
//  Copyright (c) 2014 Shuangyang Yang
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(HPX_COMPONENTS_IO_SERVER_LOCAL_FILE_HPP_AUG_27_2014_1200AM)
#define HPX_COMPONENTS_IO_SERVER_LOCAL_FILE_HPP_AUG_27_2014_1200AM

#include <hpx/config.hpp>
#if !defined(HPX_COMPUTE_DEVICE_CODE)
#include <hpx/include/actions.hpp>
#include <hpx/include/components.hpp>

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
namespace hpx::io::server
        {
            // local file class
            // uses C style file APIs
            class local_file
                : public hpx::components::locking_hook<
                      hpx::components::component_base<local_file>>

            {
            public:
                local_file() : current_buff_size(0)
                {
                    fp_ = NULL;
                    file_name_.clear();
                }

                ~

                    local_file()
                {
                }

                void open(std::string const &name, std::string const &mode)
                {
                    hpx::parallel::execution::io_pool_executor exec;
                    hpx::parallel::execution::post(exec, hpx::bind(&local_file::open_work, this, name, mode));
                }

                void open_work(std::string const &name, std::string const &mode)
                {
                    if (fp_ != NULL)
                    {
                        close();
                    }
                    fp_ = fopen(name.c_str(), mode.c_str());
                    file_name_ = name;
                }

                bool is_open() const
                {
                    return fp_ != NULL;
                }

                void close()
                {
                    hpx::parallel::execution::io_pool_executor exec;
                    hpx::parallel::execution::post(exec, hpx::bind(&local_file::close_work, this));
                }

                void close_work()
                {
                    if (fp_ != NULL)
                    {
                        std::fclose(fp_);
                        fp_ = NULL;
                    }
                    file_name_.clear();
                }

                int remove_file(std::string const &file_name)
                {
                    int result;
                    {
                        hpx::parallel::execution::io_pool_executor exec;
                        hpx::parallel::execution::post(exec, hpx::bind(&local_file::remove_file_work, this, file_name, std::ref(result)));
                    }
                    return result;
                }

                void remove_file_work(std::string const &file_name, int &result)
                {
                    result = std::remove(file_name.c_str());
                }

                std::vector<char> read(size_t const count)
                {
                    std::vector<char> result;
                    {
                        hpx::parallel::execution::io_pool_executor exec;
                        hpx::parallel::execution::async_execute(exec, hpx::bind(&local_file::read_work, this, count, std::ref(result))).get();
                    }
                    return result;
                }

                void read_work(size_t const count, std::vector<char> &result)
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

                std::vector<char> pread(size_t const count, off_t const offset)
                {
                    std::vector<char> result;
                    {
                        hpx::parallel::execution::io_pool_executor exec;
                        hpx::parallel::execution::async_execute(exec, hpx::bind(&local_file::pread_work, this, count, offset, std::ref(result))).get();
                    }
                    return result;
                }

                void pread_work(size_t const count, off_t const offset,
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

                ssize_t lazy_write_flush() {
                    ssize_t result = 0;
                    {
                        hpx::parallel::execution::io_pool_executor exec;
                        for (const auto& write_chunk : lazy_writes) {
                            auto buf = write_chunk.second;
                            auto offset = write_chunk.first;
                            hpx::parallel::execution::sync_execute(exec, hpx::bind(&local_file::pwrite_work, this, buf,
                                                                                    offset, std::ref(result)));
                        }
                    }
                    return result;
                }

                ssize_t write(std::vector<char> const &buf)
                {
                    ssize_t result = 0;
                    {
                        hpx::parallel::execution::io_pool_executor exec;
                        hpx::parallel::execution::async_execute(exec, hpx::bind(&local_file::write_work, this, buf, std::ref(result))).get();
                    }
                    return result;
                }

                void write_work(std::vector<char> const &buf,
                                ssize_t &result)
                {
                    if (fp_ == NULL || buf.empty())
                    {
                        return;
                    }
                    result = fwrite(buf.data(), 1, buf.size(), fp_);
                }

                ssize_t pwrite(std::vector<char> const &buf,
                               off_t const offset)
                {
                    lazy_writes.emplace_back(offset, buf);
                    current_buff_size += buf.size();
                    if (current_buff_size > chunk_size) {
                            // Flush buffer and write.

                    }

//                    ssize_t result = 0;
//                    {
//                        hpx::parallel::execution::io_pool_executor exec;
//                        hpx::parallel::execution::async_execute(exec, hpx::bind(&local_file::pwrite_work, this, buf, offset, std::ref(result))).get();
//                    }
//                    return result;
                    return 0;
                }

                void pwrite_work(std::vector<char> const &buf,
                                 off_t const offset, ssize_t &result)
                {
                    if (fp_ == NULL || buf.empty()
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

                int lseek(off_t const offset, int const whence)
                {
                    int result;
                    {
                        hpx::parallel::execution::io_pool_executor exec;
                        hpx::parallel::execution::async_execute(exec, hpx::bind(&local_file::lseek_work, this, offset, whence, std::ref(result))).get();
                    }
                    return result;
                }

                void lseek_work(off_t const offset, int const whence, int &result)
                {
                    if (fp_ == NULL)
                    {
                        result = -1;
                        return;
                    }

                    result = fseek(fp_, offset, whence);
                }

                off_t tell()
                {
                    off_t result;
                    {
                        hpx::parallel::execution::io_pool_executor exec;
                        hpx::parallel::execution::async_execute(exec, hpx::bind(&local_file::tell_work, this, std::ref(result))).get();
                    }
                    return result;
                }

                void tell_work(off_t &result)
                {
                    if (fp_ == NULL)
                    {
                        result = -1;
                        return;
                    }

                    result = ftell(fp_);
                }

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
                HPX_DEFINE_COMPONENT_ACTION(local_file, tell);
                HPX_DEFINE_COMPONENT_ACTION(local_file, lazy_write_flush);

            private:
                typedef hpx::components::managed_component_base<local_file> base_type;

                std::FILE *fp_;
                std::string file_name_;

                std::vector<std::pair<off_t, std::vector<char>>> lazy_writes;
                size_t chunk_size = 4 * 1024; // 4 kbs
                size_t current_buff_size;
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
HPX_REGISTER_ACTION_DECLARATION(hpx::io::server::local_file::tell_action,
                                local_file_tell_action)
HPX_REGISTER_ACTION_DECLARATION(hpx::io::server::local_file::lazy_write_flush_action,
                                local_file_lazy_write_flush_action)


HPX_REGISTER_ACTION_DECLARATION(
        hpx::lcos::base_lco_with_value<
        std::vector<std::vector<double>>>::set_value_action,
        set_value_action_vector_vector_double)

HPX_REGISTER_ACTION_DECLARATION(
        hpx::lcos::base_lco_with_value<
        std::vector<double>>::set_value_action,
        set_value_action_vector_double)

#endif
#endif