cmake_minimum_required (VERSION 3.12)
project (BoostFileStorage)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
FIND_PACKAGE(Boost REQUIRED COMPONENTS system)
IF(Boost_FOUND)
	INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
	LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
ENDIF(Boost_FOUND)

file(GLOB SOURCES "./*.cpp")

add_library(common STATIC ${SOURCES})
target_link_libraries(common ${Boost_LIBRARIES})
if(WIN32)
	target_compile_definitions(common PRIVATE "_WIN32_WINNT=0x0600")
endif(WIN32)
