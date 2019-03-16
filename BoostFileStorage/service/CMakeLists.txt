cmake_minimum_required (VERSION 3.12)
project (BoostFileStorage)

if(WIN32)
	file(GLOB SOURCES "*.cpp")
	file(GLOB HEADERS "*.h")

	source_group("Sources" FILES ${SOURCES})
	source_group("Headers" FILES ${HEADERS})

	add_executable(service ${HEADERS} ${SOURCES})
	target_link_libraries(service ${Boost_LIBRARIES} common server)

	target_compile_definitions(service PRIVATE "_WIN32_WINNT=0x0600")
else(WIN32)
	message(FATAL_ERROR "You can compile this service only for Windows platform")
endif(WIN32)
