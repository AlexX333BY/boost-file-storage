cmake_minimum_required (VERSION 3.12)
project (BoostFileStorage)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost REQUIRED COMPONENTS system)
if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
	link_directories(${Boost_LIBRARY_DIRS})
endif(Boost_FOUND)

file(GLOB SOURCES "*.cpp" "*.h")

add_library(common STATIC ${SOURCES})
target_link_libraries(common ${Boost_LIBRARIES})
if(WIN32)
	target_compile_definitions(common PRIVATE "_WIN32_WINNT=0x0600")
endif(WIN32)
