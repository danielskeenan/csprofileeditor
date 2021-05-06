cmake_minimum_required(VERSION 3.19)
project(csprofileeditor
    VERSION 0.0.1
    DESCRIPTION "Edit fixture profiles for ETC ColorSource Console"
    HOMEPAGE_URL "https://dankeenan.org"
    )
set(PROJECT_DISPLAY_NAME "CS Profile Editor")
set(PROJECT_AUTHOR "Dan Keenan")
set(PROJECT_ORGANIZATION_NAME "dankeenan")
set(PROJECT_ORGANIZATION_DOMAIN "dankeenan.org")

# Compatible with ETC's profile editor version
set(ETC_EDITOR_COMPAT "1.1.1.9.0.4")

set(CMAKE_CXX_STANDARD 17)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR}/include)

# This is more portable across compilers compared to other methods
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    add_compile_definitions(PLATFORM_LINUX)
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    add_compile_definitions(PLATFORM_WINDOWS)
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    add_compile_definitions(PLATFORM_MACOS)
endif ()

add_subdirectory(src)

if (BUILD_TESTING)
    enable_testing()
    add_subdirectory(test)
endif ()

set(BUILD_PACKAGE Off CACHE BOOL "Create packages, installers, etc.")
if (BUILD_PACKAGE)
    include(cmake/install.cmake)
endif ()
