add_library(
    foo_nowplaying2 SHARED
    main.cpp
)

target_link_libraries(
    foo_nowplaying2
    PRIVATE foobar2000 Shlwapi.lib
)

# Link statically, the foobar2000 sdk doesn't do it
#[[
set_property(
    TARGET foo_nowplaying2 PROPERTY
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)
]]
