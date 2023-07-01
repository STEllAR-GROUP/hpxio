# Copyright (c) 2015 Alireza Kheirkhahan
# Copyright (c) 2014 Shuangyang Yang
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

###############################################################################
# Local File System
###############################################################################
set(ROOT "${hpxio_SOURCE_DIR}/hpxio")

if(HPX_DEFAULT_BUILD_TARGETS)
  add_hpx_component(local_file
    FOLDER "Core/Components"
    HEADER_ROOT ${ROOT}
    SOURCES local_file.cpp
    ESSENTIAL)
  add_hpx_component(io_dispatcher
          FOLDER "Core/Components"
          HEADER_ROOT ${ROOT}
          SOURCES io_dispatcher.cpp ../hpxio/server/configuration.cpp
          ESSENTIAL)
else()
  add_hpx_component(local_file
    FOLDER "Core/Components"
    HEADER_ROOT ${ROOT}
    SOURCES local_file.cpp
    )
  add_hpx_component(io_dispatcher
          FOLDER "Core/Components"
          HEADER_ROOT ${ROOT}
          SOURCES io_dispatcher.cpp ../hpxio/server/configuration.cpp
          )
endif()