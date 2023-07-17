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
    size_t num_instances = vm["n"].as<size_t>();

    hpx::cout << "Number of localities: " << hpx::get_num_localities().get() << std::endl;

    // reading test
    {
        hpx::cout << "trying to create io_dispatcher with path:" << path << std::endl;
        // create io_dispatcher instance
        hpx::io::io_dispatcher comp(path, "r", "/hpxio/io_dispatcher", num_instances);
        hpx::cout << "io_dispatcher created" << std::endl;

        std::vector<char> read = comp.read_at(0, 10000);
        std::vector<char> read_async = comp.read_at_async(0, 100000).get();

        hpx::cout << "read:" << std::endl;
        for (auto c: read)
            hpx::cout << c;
        hpx::cout << std::endl;

        hpx::cout << "read async:" << std::endl;
        for (auto c: read_async)
            hpx::cout << c;
        hpx::cout << std::endl;
    }

    // writing test
    {
        hpx::cout << "trying to create write io_dispatcher:" << path << std::endl;
        hpx::io::io_dispatcher comp_write("./test.out", "w", "/hpxio/io_dispatcher", num_instances);


        hpx::cout << "writing into file \"test.out\"" << std::endl;
        std::vector<char> data;

        srand(time(NULL));
        for (int i = 0; i < 100; ++i) {
            data.push_back('a' + (rand() % 26));
        }

        comp_write.write_at_async(0, data).get();

        hpx::cout << "file written: ";
        for (auto c: data)
            hpx::cout << c;
        hpx::cout << std::endl;
    }

    hpx::cout << "test finished" << std::endl;
    return hpx::finalize();
}

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    // Configure application-specific options
    hpx::program_options::options_description
            desc_commandline("Usage: " HPX_APPLICATION_STRING " [options]");

    desc_commandline.add_options()
            ( "path" , hpx::program_options::value<std::string>()->default_value(std::string("/home/arnav/project/gsoc/test_files/new.txt")),
              "file path to place the testing files.")
            ( "n", hpx::program_options::value<std::size_t>()->default_value(1),
              "number of local_file instances to create.")
            ;
    hpx::init_params init_args;
    init_args.desc_cmdline = desc_commandline;
    // Initialize and run HPX
    return hpx::init(argc, argv, init_args);
}

