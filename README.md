# HPXIO - Library for asynchronous I/O

HPXIO is a library for asynchronous I/O written in the HPX runtime. 
It can be used in any HPX project to perform asynchronous I/O operations in multiple nodes.

HPXIO utilises several HPX facilities such as `futurization`, `components and acitons`,
`serialization`, etc. to provide a simple and easy to use interface for asynchronous I/O.

Part of the development for this library was done as part of the Google Summer of Code 2023 program.

## Building HPXIO

The library can be built by using the cmake file in the root directory.
The following commands can be used to build the library:

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DHPX_DIR=/path/to/hpx/lib/cmake/HPX ..
make all -j
```

## Using HPXIO

The library can be used by including the header files like this.

```cpp
    #include <hpxio/io_dispatcher.hpp>
```

This exposes the `io_dispatcher` class which can be used to perform asynchronous I/O operations.
The class has the following api

```cpp
    io_dispatcher(path, io_mode, sym_name, num_instances, chunk_size); (constructor)
    io_dispatcher.read_at_async(offset, size);
    io_dispatcher.read_at(offset, size);
    io_dispatcher.write_at_async(offset, size);
    io_dispatcher.write_at(offset, size);
    io_dispatcher.seek(offset, whence);
    io_dispatcher.tell();
```

The `_async` functions return a future.
The `_at` functions perform the operation at the given offset.
The api is similar to posix I/O.

## Examples

The `examples` directory contains some examples of how to use the library.

For the `io_dispatcher` class, the `io_dispatcher_test.cpp` file contains an example of how to use the class.
Also refer to the `CMakeLists.txt` file in the examples directory to see how to link the library in your project.

## Testing

The `io_dispatcher_test.cpp` file can be run as follows to test the class

```bash
    ./io_dispatcher_test --path=/path/to/file/to/read \
                      --n=number_of_instances \
                      --read_size=size_of_reads \
                      --chunk_size=size_of_chunk_for_io_dispatcher \
                      --write_size=size_of_writes \
                      --n-ops=number_of_io_operations
```

The `--path` argument is mandatory. The other arguments have default values.
