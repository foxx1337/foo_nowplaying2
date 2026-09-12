cmake_minimum_required(VERSION 3.24)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

project(foo_nowplaying2)

set(CMAKE_CXX_STANDARD 20)
if(NOT DEFINED CMAKE_MSVC_RUNTIME_LIBRARY)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()
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

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/footilla/CMakeLists.txt"
   OR NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/footilla/Scintilla/License.txt")
    message(FATAL_ERROR
        "Footilla is missing or incomplete. From the foo_nowplaying2 directory, run:\n"
        "  git submodule update --init --recursive")
endif()

# Win32 and x64 need the Visual Studio 2019 v142 platform toolset
# as the newer one wrongly generates AVX code even when not requested.
# YOLOing with v145, regardless.
set(FOO_NOWPLAYING2_PLATFORM_TOOLSET "v145" CACHE STRING
    "Win32/x64 platform toolset; empty uses the generator default (developer override)")

option(FOOTILLA_BUILD_DEMO "Build the Footilla editor playground" OFF)
option(FOOTILLA_BUILD_TESTS "Build the Footilla tests" OFF)
add_subdirectory(footilla)
foreach(_target footilla footilla_scintilla footilla_demo footilla_language_tests
                footilla_control_tests footilla_visual_tests)
    if(TARGET ${_target})
        set_property(TARGET ${_target} PROPERTY CXX_STANDARD 17)
    endif()
endforeach()
add_subdirectory(src)

option(FOO_NOWPLAYING2_BUILD_TESTS "Build the title-format editor dialog tests" OFF)
if(FOO_NOWPLAYING2_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
    get_property(_foo_nowplaying2_test_targets DIRECTORY tests PROPERTY BUILDSYSTEM_TARGETS)
endif()

if((platform STREQUAL "Win32" OR platform STREQUAL "x64")
   AND FOO_NOWPLAYING2_PLATFORM_TOOLSET)
    set_target_properties(
        foo_nowplaying2 footilla footilla_scintilla
        PROPERTIES
        VS_PLATFORM_TOOLSET "${FOO_NOWPLAYING2_PLATFORM_TOOLSET}"
    )
    foreach(_target footilla_demo footilla_language_tests footilla_control_tests footilla_visual_tests
                    ${_foo_nowplaying2_test_targets})
        if(TARGET ${_target})
            set_property(TARGET ${_target} PROPERTY
                VS_PLATFORM_TOOLSET "${FOO_NOWPLAYING2_PLATFORM_TOOLSET}")
        endif()
    endforeach()
endif()
