cmake_minimum_required(VERSION 3.4)
project(ubit LANGUAGES CXX C)
#set(CMAKE_CXX_STANDARD 14)

include(cmake/ubit.cmake)

#if(WITH_CXX11ABI)
#    add_definitions(-D_GLIBCXX_USE_CXX11_ABI=1)
#else()
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
#endif()

set(CMAKE_CONFIGURATION_TYPES Debug Release RelWithDebugInfo Sanitize)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wno-unused-parameter -Wno-unused-variable -Wno-extra -Wno-shadow")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -fvisibility=hidden -Wall -Wextra -Wshadow -Wno-variadic-macros -Wno-unknown-pragmas")

if (WITH_ERROR)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Werror")
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_SANITIZE "${CMAKE_CXX_FLAGS_SANITIZE} -O1 -g -fno-omit-frame-pointer -fno-optimize-sibling-calls")

if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=unknown-warning-option")
elseif(CMAKE_COMPILER_IS_GNUCXX)
    # https://svn.boost.org/trac/boost/ticket/9240
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fext-numeric-literals")
endif()

# Technique from https://crascit.com/2016/04/09/using-ccache-with-cmake/
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(C_LAUNCHER   "${CCACHE_PROGRAM}")
    set(CXX_LAUNCHER "${CCACHE_PROGRAM}")

	# Support Unix Makefiles and Ninja
	configure_file(scripts/launch-c.in   launch-c   @ONLY)
	configure_file(scripts/launch-cxx.in launch-cxx @ONLY)

	set(CMAKE_C_COMPILER_LAUNCHER        "${CMAKE_BINARY_DIR}/launch-c")
	set(CMAKE_CXX_COMPILER_LAUNCHER      "${CMAKE_BINARY_DIR}/launch-cxx")

    execute_process(COMMAND chmod a+rx "${CMAKE_BINARY_DIR}/launch-c" "${CMAKE_BINARY_DIR}/launch-cxx")

    if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
        # ccache splits up the compile steps, so we end up with unused arguments in some steps.
        # Clang also thinks that ccache isn't interactive, so we explicitly need to enable color.
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qunused-arguments -fcolor-diagnostics")
    endif()
else()
    message(STATUS "Can't find ccache — consider installing ccache to improve recompilation performance")
endif()

include(cmake/config.cmake)
include(cmake/core-files.cmake)
include(cmake/core.cmake)

include(cmake/demos.cmake)

