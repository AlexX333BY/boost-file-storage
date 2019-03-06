cmake_minimum_required (VERSION 3.12)
project (BoostFileStorage)

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h")

source_group("Sources" FILES ${SOURCES})
source_group("Headers" FILES ${HEADERS})

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost REQUIRED COMPONENTS system)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

add_library(common STATIC ${HEADERS} ${SOURCES})
target_link_libraries(common ${Boost_LIBRARIES})
if(WIN32)
	target_compile_definitions(common PRIVATE "_WIN32_WINNT=0x0600")
endif(WIN32)
