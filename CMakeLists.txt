cmake_minimum_required(VERSION 3.15)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

project(foo_nowplaying2)

set(CMAKE_CXX_STANDARD 20)

if(CMAKE_GENERATOR_PLATFORM)
    set(platform ${CMAKE_GENERATOR_PLATFORM})
else()
    set(platform ${CMAKE_VS_PLATFORM_NAME_DEFAULT})
endif()
string(TOUPPER "${platform}" uplatform)

if(${platform} STREQUAL "Win32")
    set(platform_DIR "")
else()
    set(platform_DIR "${platform}/")
endif()

add_compile_options(/W3)

find_package(foobar2000)

add_subdirectory(src)
