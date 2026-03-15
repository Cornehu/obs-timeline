# ObsPluginHelpers.cmake
# Cross-platform detection helpers

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(OS_WINDOWS TRUE)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(OS_MACOS TRUE)
else()
    set(OS_LINUX TRUE)
endif()
