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
    set(OBS_SOURCE_DIR "$ENV{HOME}/obs-studio" CACHE PATH "OBS Studio source directory")
    set(OBS_FRAMEWORKS "/Applications/OBS.app/Contents/Frameworks" CACHE PATH "OBS frameworks directory")

    # obsconfig.h est généré par cmake dans le dossier build d'OBS.
    # On le cherche automatiquement si OBS_CONFIG_DIR n'est pas fourni.
    if(NOT DEFINED OBS_CONFIG_DIR OR OBS_CONFIG_DIR STREQUAL "")
        file(GLOB_RECURSE _obsconfig "${OBS_SOURCE_DIR}/build/obsconfig.h"
                                     "${OBS_SOURCE_DIR}/build/config/obsconfig.h")
        if(_obsconfig)
            list(GET _obsconfig 0 _obsconfig_file)
            get_filename_component(OBS_CONFIG_DIR "${_obsconfig_file}" DIRECTORY)
        else()
            set(OBS_CONFIG_DIR "${OBS_SOURCE_DIR}/build/config")
        endif()
    endif()

    find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

    target_include_directories(obs-timeline-monitor PRIVATE
        src
        ${OBS_SOURCE_DIR}/libobs
        ${OBS_SOURCE_DIR}/UI/obs-frontend-api
        ${OBS_CONFIG_DIR}
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

    # Fix Qt paths: Homebrew absolu → @rpath (Qt bundlé dans OBS)
    add_custom_command(TARGET obs-timeline-monitor POST_BUILD
        COMMAND bash -c
            "for LIB in QtCore QtGui QtWidgets; do \
                OLD=$$(otool -L \"$<TARGET_FILE:obs-timeline-monitor>\" 2>/dev/null \
                      | grep \"opt/homebrew.*$$LIB\" | awk '{print $$1}'); \
                [ -n \"$$OLD\" ] && install_name_tool -change \"$$OLD\" \
                    \"@rpath/$$LIB.framework/Versions/A/$$LIB\" \
                    \"$<TARGET_FILE:obs-timeline-monitor>\"; \
            done"
        COMMENT "Fixing Qt @rpath"
    )

# =============================================================
# Windows
# =============================================================
elseif(WIN32)
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

    install(TARGETS obs-timeline-monitor RUNTIME DESTINATION "obs-plugins/64bit")
    install(DIRECTORY data/locale DESTINATION "data/obs-plugins/obs-timeline-monitor")

# =============================================================
# Linux
# =============================================================
else()
    # Deux modes :
    # 1. find_package (installation système standard)
    # 2. Mode manuel via OBS_SOURCE_DIR + OBS_LIB_DIR (utilisé en CI)
    if(DEFINED OBS_SOURCE_DIR AND NOT OBS_SOURCE_DIR STREQUAL "")
        # Mode CI : on a les sources OBS et les libs installées
        set(OBS_LIB_DIR "/usr/lib/x86_64-linux-gnu" CACHE PATH "OBS library directory")
        set(OBS_CONFIG_DIR "${OBS_SOURCE_DIR}/build/config" CACHE PATH "obsconfig.h directory")

        target_include_directories(obs-timeline-monitor PRIVATE
            src
            ${OBS_SOURCE_DIR}/libobs
            ${OBS_SOURCE_DIR}/UI/obs-frontend-api
            ${OBS_CONFIG_DIR}
        )
        target_link_libraries(obs-timeline-monitor PRIVATE
            "${OBS_LIB_DIR}/libobs.so"
            "${OBS_LIB_DIR}/libobs-frontend-api.so"
            Qt6::Core
            Qt6::Widgets
        )
    else()
        # Mode standard : OBS installé avec cmake configs
        find_package(libobs REQUIRED)
        find_package(obs-frontend-api REQUIRED)
        target_include_directories(obs-timeline-monitor PRIVATE src)
        target_link_libraries(obs-timeline-monitor PRIVATE
            OBS::libobs
            OBS::obs-frontend-api
            Qt6::Core
            Qt6::Widgets
        )
    endif()

    find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

    install(TARGETS obs-timeline-monitor LIBRARY DESTINATION "lib/obs-plugins")
    install(DIRECTORY data/locale DESTINATION "share/obs/obs-plugins/obs-timeline-monitor")
endif()
