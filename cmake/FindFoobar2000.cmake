# Finds the foobar2000 SDK, the one compiled from the official archive
# at https://www.foobar2000.org/SDK
#
# Input variables:
#  foobar2000sdk
#  platform
#
# Output variables:
#  FOOBAR2000_INCLUDE_DIRS, contaning all of
#    FOOBAR2000_SDK_INCLUDE_DIR
#    FOOBAR2000_LIBPPUI_INCLUDE_DIR
#    FOOBAR2000_PFC_INCLUDE_DIR
#  FOOBAR2000_LIBS, containing debug and release for all of
#    FOOBAR2000_SDK_<DEBUG|RELEASE>_LIB
#    LIBPPUI_<DEBUG|RELEASE>_LIB
#    FOOBAR2000_COMPONENT_CLIENT_<DEBUG|RELEASE>_LIB
#    SHARED-<Win32|x64|ARM64>_<DEBUG|RELEASE>_LIB
#    PFC_<DEBUG|RELEASE>_LIB

find_path(
    FOOBAR2000_SDK_INCLUDE_DIR
    NAMES SDK/foobar2000.h
    HINTS
        "${foobar2000sdk}/foobar2000"
)

find_path(
    FOOBAR2000_LIBPPUI_INCLUDE_DIR
    NAMES DarkMode.h
    HINTS
        "${foobar2000sdk}/libPPUI"
)

find_path(
    FOOBAR2000_PFC_INCLUDE_DIR
    NAMES PFC/audio_sample.h
    HINTS
        "${foobar2000sdk}"
)

function(find_foobar2000_lib base_name configuration)
    string(TOUPPER "${base_name}" lib_uname)
    string(TOUPPER "${configuration}" cfg_uname)
    
    find_library(
        ${lib_uname}_${cfg_uname}_LIB
        NAMES ${base_name}
        HINTS
            "${foobar2000sdk}"
            ${_${lib_uname}_LIB_DIR}
        PATH_SUFFIXES
            "foobar2000/foo_sample/${platform_DIR}${configuration}"
            "foobar2000/foobar2000_component_client/${platform_DIR}${configuration}"
            "foobar2000/shared"
            "foobar2000/foo_sample/${platform_DIR}${configuration} FB2K/"
    )
endfunction()

function(combine_debug_optimized base_name)
    string(TOUPPER "${base_name}" base_uname)
    find_foobar2000_lib(${base_name} Debug)
    find_foobar2000_lib(${base_name} Release)
    set(${base_uname}_LIBS debug ${${base_uname}_DEBUG_LIB} optimized ${${base_uname}_RELEASE_LIB} PARENT_SCOPE)
    mark_as_advanced(${base_uname}_LIBS ${base_uname}_DEBUG_LIB ${base_uname}_RELEASE_LIB)
endfunction()

combine_debug_optimized(foobar2000_SDK)
combine_debug_optimized(foobar2000_sdk_helpers)
combine_debug_optimized(foobar2000_component_client)
combine_debug_optimized(libPPUI)
combine_debug_optimized("shared-${platform}")
combine_debug_optimized(pfc)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    foobar2000
    DEFAULT_MSG
    FOOBAR2000_SDK_LIBS
    FOOBAR2000_SDK_HELPERS_LIBS
    FOOBAR2000_COMPONENT_CLIENT_LIBS
    LIBPPUI_LIBS
    SHARED-${uplatform}_LIBS
    PFC_LIBS
    FOOBAR2000_SDK_INCLUDE_DIR
    FOOBAR2000_LIBPPUI_INCLUDE_DIR
    FOOBAR2000_PFC_INCLUDE_DIR
)

function(create_foobar2000_namespace_lib base_name include_dir)
    if(NOT TARGET foobar2000::${base_name})
        string(TOUPPER "${base_name}" base_uname)
        add_library(foobar2000::${base_name} UNKNOWN IMPORTED)
        set_target_properties(
            foobar2000::${base_name} PROPERTIES
                IMPORTED_LOCATION ${${base_uname}_RELEASE_LIB}
                IMPORTED_LOCATION_DEBUG ${${base_uname}_DEBUG_LIB}
                INTERFACE_INCLUDE_DIRECTORIES ${include_dir}
        )
    endif()
endfunction()

if(FOOBAR2000_FOUND)
    set(FOOBAR2000_INCLUDE_DIRS ${FOOBAR2000_SDK_INCLUDE_DIR} ${FOOBAR2000_PFC_INCLUDE_DIR})
    set(FOOBAR2000_LIBS ${FOOBAR2000_SDK_LIBS} ${FOOBAR2000_COMPONENT_CLIENT_LIBS} ${SHARED-${uplatform}_LIBS} ${PFC_LIBS})

    create_foobar2000_namespace_lib(foobar2000_SDK ${FOOBAR2000_SDK_INCLUDE_DIR})
    create_foobar2000_namespace_lib(foobar2000_sdk_helpers ${FOOBAR2000_SDK_INCLUDE_DIR})
    create_foobar2000_namespace_lib(foobar2000_component_client ${FOOBAR2000_SDK_INCLUDE_DIR})
    create_foobar2000_namespace_lib(libPPUI ${FOOBAR2000_LIBPPUI_INCLUDE_DIR})
    create_foobar2000_namespace_lib(shared-${platform} ${FOOBAR2000_SDK_INCLUDE_DIR})
    create_foobar2000_namespace_lib(pfc ${FOOBAR2000_PFC_INCLUDE_DIR})

    if(NOT TARGET foobar2000::foobar2000)
        add_library(foobar2000::foobar2000 INTERFACE IMPORTED)
        set_property(
            TARGET foobar2000::foobar2000 PROPERTY
            INTERFACE_LINK_LIBRARIES
                foobar2000::foobar2000_SDK
                foobar2000::foobar2000_sdk_helpers
                foobar2000::foobar2000_component_client
                foobar2000::libPPUI
                foobar2000::shared-${platform}
                foobar2000::pfc
        )
        add_library(foobar2000 ALIAS foobar2000::foobar2000)
    endif()
endif()
