cmake_minimum_required (VERSION 3.12)
project (BoostFileStorage)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost REQUIRED COMPONENTS system)
if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
	link_directories(${Boost_LIBRARY_DIRS})
endif(Boost_FOUND)

include_directories("../common")

if (NOT TARGET common)
	add_subdirectory("../common" "_deps/common")
endif()

file(GLOB SOURCES "*.cpp" "*.h")

add_library(server STATIC ${SOURCES})
target_link_libraries(server ${Boost_LIBRARIES} common)
if(WIN32)
	target_compile_definitions(server PRIVATE "_WIN32_WINNT=0x0600")
endif(WIN32)
