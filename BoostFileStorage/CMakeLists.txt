cmake_minimum_required (VERSION 3.12)
project (BoostFileStorage)
cmake_policy(SET CMP0079 NEW)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost REQUIRED COMPONENTS system)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

add_subdirectory(common)
target_link_libraries(common ${Boost_LIBRARIES})

add_subdirectory(server)
target_link_libraries(server ${Boost_LIBRARIES} common)

add_subdirectory(service)
target_link_libraries(service ${Boost_LIBRARIES} common server)

add_subdirectory(client)
target_link_libraries(client common)
