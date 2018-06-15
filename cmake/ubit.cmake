if (NOT MBGL_PLATFORM)
    if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
        set(MBGL_PLATFORM "macos")
    else()
        set(MBGL_PLATFORM "linux")
    endif()
    
    add_definitions(-I/usr/include/freetype2/)

endif()
