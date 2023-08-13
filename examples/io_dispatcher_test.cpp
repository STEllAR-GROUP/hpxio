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
#include <chrono>

class Timer {
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start_timepoint;

    void stop() const {
        auto end_point = std::chrono::high_resolution_clock::now();

        auto start = std::chrono::time_point_cast<std::chrono::microseconds>
                (m_start_timepoint).time_since_epoch().count();
        auto end = std::chrono::time_point_cast
                <std::chrono::microseconds>(end_point).time_since_epoch().count();

        auto duration = end - start;
        hpx::cout << "Took " << duration << " microseconds\n";
    }

public:
    Timer() {
        m_start_timepoint = std::chrono::high_resolution_clock::now();
    }

    ~Timer() {
        stop();
    }
};

/////////////////////////////////////////////////////////////////////////////
int hpx_main(hpx::program_options::variables_map& vm)
{
    // extract command line argument
    std::string path = vm["path"].as<std::string>();
    size_t num_instances = vm["n"].as<size_t>();
    size_t chunk_size = vm["chunk-size"].as<size_t>();
    size_t read_size = vm["read-size"].as<size_t>();
    size_t write_size = vm["write-size"].as<size_t>();
    size_t n_ops = vm["n-ops"].as<size_t>();

    size_t file_size = hpx::filesystem::file_size(path);

    hpx::cout << "Number of localities: " << hpx::get_num_localities().get() << std::endl;
//    std::vector<char> read;
    // reading test
    {
        hpx::cout << "trying to create io_dispatcher with path:" << path << std::endl;
        // create io_dispatcher instance
        Timer timer_read;
        hpx::io::io_dispatcher comp(path, "r", "/hpxio/io_dispatcher", num_instances, chunk_size);
        hpx::cout << "io_dispatcher created" << std::endl;

        for (int i = 0; i< n_ops; ++i)
            comp.read_at(rand()%file_size, read_size);
    }

    std::vector<char> data;
    // writing test
    {
        hpx::cout << "trying to create write io_dispatcher:" << path << std::endl;
        Timer timer_write;
        hpx::io::io_dispatcher comp_write("./test.out", "w", "/hpxio/io_dispatcher", num_instances);


        hpx::cout << "writing into file \"test.out\"" << std::endl;

        srand(time(NULL));
        auto generator = [](){return 'a' + (rand() % 26);};
        for (int j = 0; j < n_ops; ++j)
        {
            data.resize(write_size);
            hpx::ranges::generate(data, generator);

            comp_write.write_at_async(0, data).get();
        }
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
            ( "chunk-size", hpx::program_options::value<std::size_t>()->default_value(4 * 1024),
              "chunk size to use for the file operations.")
            ("read-size", hpx::program_options::value<std::size_t>()->default_value(4 * 1024),
             "read size to use for the file operations.")
            ("write-size", hpx::program_options::value<std::size_t>()->default_value(4 * 1024),
             "write size to use for the file operations.")
            ("n-ops", hpx::program_options::value<std::size_t>()->default_value(10000),
             "number of operations to perform.")
            ;
    hpx::init_params init_args;
    init_args.desc_cmdline = desc_commandline;
    // Initialize and run HPX
    return hpx::init(argc, argv, init_args);
}

