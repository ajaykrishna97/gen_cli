cmake_minimum_required(VERSION 3.00)

set( CMAKE_CXX_COMPILER "g++" )
set( CMAKE_C_COMPILER   "gcc" )

project(gen_cli)
 
set(SOURCES ./CLI/src/main.c ./CLI/src/cli.c ./helper/src/helper.c ./CLI/src/cla_parser.c)
add_executable(gen_cli ${SOURCES})

target_include_directories(gen_cli PUBLIC ./CLI/include/ ./helper/include)

if (UNIX)
	find_package (Threads)
    target_link_libraries (gen_cli ${CMAKE_THREAD_LIBS_INIT})
endif (UNIX)


