cmake_minimum_required (VERSION 3.12)
project (BoostFileStorage)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost REQUIRED COMPONENTS system)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

add_subdirectory(common)
add_subdirectory(server)
add_subdirectory(service)
add_subdirectory(client)
