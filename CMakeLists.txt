cmake_minimum_required(VERSION 3.16)

project(obs-timeline-monitor VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- OBS Plugin helpers ---
include(cmake/ObsPluginHelpers.cmake OPTIONAL)

# Find OBS Studio
find_package(libobs REQUIRED)
find_package(obs-frontend-api REQUIRED)

# Find Qt6 (bundled with OBS)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Source files
set(PLUGIN_SOURCES
    src/plugin-main.cpp
    src/timeline-dock.cpp
    src/source-monitor.cpp
)

set(PLUGIN_HEADERS
    src/timeline-dock.hpp
    src/source-monitor.hpp
)

add_library(obs-timeline-monitor MODULE
    ${PLUGIN_SOURCES}
    ${PLUGIN_HEADERS}
)

target_link_libraries(obs-timeline-monitor
    PRIVATE
        OBS::libobs
        OBS::obs-frontend-api
        Qt6::Core
        Qt6::Widgets
)

target_include_directories(obs-timeline-monitor PRIVATE src)

# --- Cross-platform install ---
if(OS_WINDOWS)
    set_target_properties(obs-timeline-monitor PROPERTIES SUFFIX ".dll")
    install(TARGETS obs-timeline-monitor
        RUNTIME DESTINATION "obs-plugins/64bit"
    )
elseif(OS_MACOS)
    set_target_properties(obs-timeline-monitor PROPERTIES SUFFIX ".dylib")
    install(TARGETS obs-timeline-monitor
        LIBRARY DESTINATION "obs-plugins"
    )
else() # Linux
    install(TARGETS obs-timeline-monitor
        LIBRARY DESTINATION "lib/obs-plugins"
    )
endif()

# Locale files
install(DIRECTORY data/locale
    DESTINATION "data/obs-plugins/obs-timeline-monitor/locale"
)
