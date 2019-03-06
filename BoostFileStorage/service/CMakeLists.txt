cmake_minimum_required (VERSION 3.12)
project (BoostFileStorage)

if(WIN32)
	set(Boost_USE_STATIC_LIBS ON)
	set(Boost_USE_MULTITHREADED ON)
	find_package(Boost REQUIRED COMPONENTS system)
	if(Boost_FOUND)
		include_directories(${Boost_INCLUDE_DIRS})
		link_directories(${Boost_LIBRARY_DIRS})
	endif(Boost_FOUND)

	include_directories("../common" "../server")
	if (NOT TARGET common)
		add_subdirectory("../common" "_deps/common")
	endif()
	
	if (NOT TARGET server)
		add_subdirectory("../server" "_deps/server")
	endif()

	add_executable(service Main.cpp)
	target_link_libraries(service ${Boost_LIBRARIES} common server)

	target_compile_definitions(service PRIVATE "_WIN32_WINNT=0x0600")
else(WIN32)
	message(FATAL_ERROR "You can compile this service only for Windows platform")
endif(WIN32)
