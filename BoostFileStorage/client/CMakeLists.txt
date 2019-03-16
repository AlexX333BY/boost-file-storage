cmake_minimum_required (VERSION 3.12)
project (BoostFileStorage)

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h")

source_group("Sources" FILES ${SOURCES})
source_group("Headers" FILES ${HEADERS})

find_package(wxWidgets REQUIRED COMPONENTS core base net)
include(${wxWidgets_USE_FILE})

if(WIN32)
	add_executable(client WIN32 ${HEADERS} ${SOURCES})
else(WIN32)
	add_executable(client ${HEADERS} ${SOURCES})
endif(WIN32)
target_link_libraries(client ${wxWidgets_LIBRARIES} common)
