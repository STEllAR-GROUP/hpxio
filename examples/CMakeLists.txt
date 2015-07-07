# Copyright (c) 2015 Alireza Kheirkhahan
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

set(example_programs
	diskperf_hpx_sync_local_file)
set(diskperf_hpx_sync_local_file_FLAGS DEPENDENCIES
	iostreams_component local_file_component)

foreach(example_program ${example_programs})
	set(sources ${example_program}.cpp)
	source_group("Source Files" FILES ${sources})

	add_hpx_executable(${example_program}
	SOURCES ${sources}
	${${example_program}_FLAGS}
	FOLDER "Examples/${example_program}")
endforeach()
