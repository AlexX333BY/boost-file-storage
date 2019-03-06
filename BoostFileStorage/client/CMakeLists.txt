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

find_package(wxWidgets REQUIRED COMPONENTS core base net)
include(${wxWidgets_USE_FILE})

if(NOT TARGET common)
	include_directories("../common")
	add_subdirectory("../common" "_deps/common")
endif()

if(WIN32)
	add_executable(client WIN32 ${HEADERS} ${SOURCES})
else(WIN32)
	add_executable(client ${HEADERS} ${SOURCES})
endif(WIN32)
target_link_libraries(client ${wxWidgets_LIBRARIES} common)
