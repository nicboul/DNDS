add_executable(dnc
	dnc.c
	main.c
	session.c
)

include_directories("${CMAKE_SOURCE_DIR}/../libdnds/src/protocol/")
target_link_libraries(dnc
	${CMAKE_SOURCE_DIR}/../libdnds/src/libdnds.so
)

install(TARGETS dnc
	DESTINATION bin)