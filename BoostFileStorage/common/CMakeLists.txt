cmake_minimum_required (VERSION 3.12)
project (BoostFileStorage)

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h")

source_group("Sources" FILES ${SOURCES})
source_group("Headers" FILES ${HEADERS})

add_library(common STATIC ${HEADERS} ${SOURCES})
target_link_libraries(common ${Boost_LIBRARIES})
if(WIN32)
	target_compile_definitions(common PRIVATE "_WIN32_WINNT=0x0600")
endif(WIN32)
