add_library(
    foo_nowplaying2 SHARED
    main.cpp
    preferences.cpp
    nowplaying.cpp
    resource.h
    preferences.h
    nowplaying.h
    foo_nowplaying2.rc
)

target_link_libraries(
    foo_nowplaying2
    PRIVATE foobar2000 wtl Shlwapi.lib
)

source_group("Resource Files" foo_nowplaying2.rc)

# Link statically, the foobar2000 sdk doesn't do it
#[[
set_property(
    TARGET foo_nowplaying2 PROPERTY
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)
]]
