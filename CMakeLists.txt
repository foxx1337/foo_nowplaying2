cmake_minimum_required(VERSION 3.15)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

project(foo_nowplaying2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /DUNICODE /D_UNICODE")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DUNICODE /D_UNICODE")

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

add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/W3>)
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP>)

find_package(foobar2000)
find_package(wtl)

add_subdirectory(src)

# Win32 and x64 need the Visual Studio 2019 v142 platform toolset
# as the newer one wrongly generates AVX code even when not requested.
if(NOT ${platform} STREQUAL "ARM64EC")
    set_target_properties(
        foo_nowplaying2 
        PROPERTIES
        VS_PLATFORM_TOOLSET v142
    )
endif()
