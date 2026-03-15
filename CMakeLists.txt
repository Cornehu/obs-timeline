cmake_minimum_required(VERSION 3.16)
project(obs-timeline-monitor VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(PLUGIN_SOURCES
    src/plugin-main.cpp
    src/timeline-dock.cpp
    src/source-monitor.cpp
)
set(PLUGIN_HEADERS
    src/timeline-dock.hpp
    src/source-monitor.hpp
)

add_library(obs-timeline-monitor MODULE ${PLUGIN_SOURCES} ${PLUGIN_HEADERS})

# =============================================================
# macOS
# =============================================================
if(APPLE)
    set(OBS_APP "/Applications/OBS.app" CACHE PATH "Path to OBS.app")
    set(OBS_SOURCE_DIR "$ENV{HOME}/obs-studio" CACHE PATH "OBS Studio source (for headers)")
    set(OBS_FRAMEWORKS "${OBS_APP}/Contents/Frameworks")

    find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

    target_include_directories(obs-timeline-monitor PRIVATE
        src
        ${OBS_SOURCE_DIR}/libobs
        ${OBS_SOURCE_DIR}/frontend/api
        ${OBS_SOURCE_DIR}/UI/obs-frontend-api
        ${OBS_SOURCE_DIR}/build/config
    )

    target_link_libraries(obs-timeline-monitor PRIVATE
        "-F${OBS_FRAMEWORKS}" "-framework libobs"
        "${OBS_FRAMEWORKS}/obs-frontend-api.dylib"
        Qt6::Core
        Qt6::Widgets
    )

    set_target_properties(obs-timeline-monitor PROPERTIES
        PREFIX ""
        SUFFIX ""
        MACOSX_RPATH ON
        BUILD_RPATH   "${OBS_FRAMEWORKS}"
        INSTALL_RPATH "@loader_path/../../../../Frameworks"
    )

    # Redirect Qt links from Homebrew absolute paths → @rpath (OBS bundled Qt)
    add_custom_command(TARGET obs-timeline-monitor POST_BUILD
        COMMAND bash "${CMAKE_SOURCE_DIR}/cmake/fix_qt_rpath.sh"
                     "$<TARGET_FILE:obs-timeline-monitor>"
        COMMENT "Fixing Qt @rpath for OBS bundled Qt"
    )

# =============================================================
# Windows
# =============================================================
elseif(WIN32)
    # OBS installs CMake configs to:
    #   C:\Program Files\obs-studio\cmake\
    # Set CMAKE_PREFIX_PATH or pass -DlibobS_DIR / -Dobs-frontend-api_DIR
    find_package(libobs REQUIRED)
    find_package(obs-frontend-api REQUIRED)
    find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

    target_include_directories(obs-timeline-monitor PRIVATE src)

    target_link_libraries(obs-timeline-monitor PRIVATE
        OBS::libobs
        OBS::obs-frontend-api
        Qt6::Core
        Qt6::Widgets
    )

    set_target_properties(obs-timeline-monitor PROPERTIES SUFFIX ".dll")

    install(TARGETS obs-timeline-monitor
        RUNTIME DESTINATION "obs-plugins/64bit"
    )
    install(DIRECTORY data/locale
        DESTINATION "data/obs-plugins/obs-timeline-monitor"
    )

# =============================================================
# Linux
# =============================================================
else()
    find_package(libobs REQUIRED)
    find_package(obs-frontend-api REQUIRED)
    find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

    target_include_directories(obs-timeline-monitor PRIVATE src)

    target_link_libraries(obs-timeline-monitor PRIVATE
        OBS::libobs
        OBS::obs-frontend-api
        Qt6::Core
        Qt6::Widgets
    )

    install(TARGETS obs-timeline-monitor
        LIBRARY DESTINATION "lib/obs-plugins"
    )
    install(DIRECTORY data/locale
        DESTINATION "share/obs/obs-plugins/obs-timeline-monitor"
    )
endif()
