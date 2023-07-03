//
// Created by arnav on 25/6/23.
//

////////////////////////////////////////////////////////////////////////////////
//  Copyright (c)      2014 Shuangyang Yang
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
////////////////////////////////////////////////////////////////////////////////


#include <hpxio/io_dispatcher.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/iostream.hpp>


/////////////////////////////////////////////////////////////////////////////
int hpx_main(hpx::program_options::variables_map& vm)
{
    // extract command line argument
    std::string path = vm["path"].as<std::string>();

    hpx::cout << "Num,ber of localities: " << hpx::get_num_localities().get() << std::endl;

    hpx::cout << "trying to create io_dispatcher with path:" << path << std::endl;
    // create io_dispatcher instance
    hpx::io::io_dispatcher *comp = new hpx::io::io_dispatcher("/hpxio/io_dispatcher", 2);
    hpx::cout << "io_dispatcher created" << std::endl;
    hpx::cout << "trying to open file" << std::endl;
    // open file
    comp->open(path, "r");

    hpx::cout << "file opened" << std::endl;
    std::vector<char> read = comp->read_at(0, 10);
    std::vector<char> read_async = comp->read_at_async(0, 10).get();

    hpx::cout << "read:" << std::endl;
    for (auto c : read)
        hpx::cout << c;
    hpx::cout << std::endl;

    hpx::cout << "read asyc:" << std::endl;
    for (auto c : read_async)
        hpx::cout << c;
    hpx::cout << std::endl;

    return hpx::finalize();
}

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    // Configure application-specific options
    hpx::program_options::options_description
            desc_commandline("Usage: " HPX_APPLICATION_STRING " [options]");

    desc_commandline.add_options()
            ( "path" , hpx::program_options::value<std::string>()->default_value(std::string("some_path")),
              "file path to place the testing files.")
            ;
    hpx::init_params init_args;
    init_args.desc_cmdline = desc_commandline;
    // Initialize and run HPX
    return hpx::init(argc, argv, init_args);
}

