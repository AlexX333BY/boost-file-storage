cmake_minimum_required (VERSION 3.12)
project (BoostFileStorage)

find_package(wxWidgets REQUIRED COMPONENTS core base net)
include(${wxWidgets_USE_FILE})

if(NOT TARGET common)
	include_directories("../common")
	add_subdirectory("../common" "_deps/common")
endif()

add_executable(client Main.cpp)
target_link_libraries(client ${wxWidgets_LIBRARIES} common)
