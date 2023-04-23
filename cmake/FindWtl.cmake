# Finds the WTL include library, from the WTL source available
# at https://sourceforge.net/projects/wtl/
#
# Input variables:
#  wtl
#
# Output variables:
#  WTL_INCLUDE_DIR

find_path(
    WTL_INCLUDE_DIR
    NAMES atlapp.h
    HINTS
        "${wtl}/Include"
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    wtl
    WTL_INCLUDE_DIR
)

if(WTL_FOUND)
    if(NOT TARGET wtl::wtl)
        add_library(wtl::wtl INTERFACE IMPORTED)
        set_target_properties(
            wtl::wtl PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES ${WTL_INCLUDE_DIR}
        )
        add_library(wtl ALIAS wtl::wtl)
    endif()
endif()
