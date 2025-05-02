include_directories(../../_libs/xiAPI)

if(UNIX)
    link_libraries(m3api)
else()
    # Windows
    if (${CMAKE_VS_PLATFORM_NAME} STREQUAL "x64")
        link_libraries(xiapi64)
    else()
        link_libraries(xiapi32)
    endif()
    add_definitions(-DWIN32)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    link_directories(../../../bin)
endif()

