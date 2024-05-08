cmake_minimum_required(VERSION 3.5)

project(my-jobz VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)
include_directories(headers)

set(ICON_RESOURCE ${CMAKE_CURRENT_SOURCE_DIR}/resources/application.ico)
set(SQLITE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources/sqlite)
set(RESOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/resources/myjobz.rc)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql LinguistTools)

set(TS_FILES
        resources/translations/myjobz_pt_BR.ts
        resources/translations/myjobz_pt_BR.qm
)

set(RESOURCES
        ${TS_FILES}
        ${RESOURCE_FILES}
        ${ICON_RESOURCE}
)

set(PROJECT_SOURCES
        main.cpp
        src/main_window.cpp
        src/details_dialog.cpp
        src/proxy_model.cpp
        src/database_manager.cpp
        resources/resources.qrc
)

set(PROJECT_HEADERS
        headers/main_window.hpp
        headers/details_dialog.hpp
        headers/proxy_model.hpp
        headers/database_manager.hpp
)

set(PROJECT_FORMS
        ui/main_window.ui
        ui/details_dialog.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(my-jobz
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
        ${PROJECT_FORMS}
        ${RESOURCES}
    )
else()
    if(ANDROID)
        add_library(my-jobz SHARED
            ${PROJECT_SOURCES}
            ${PROJECT_HEADERS}
            ${PROJECT_FORMS}
            ${RESOURCES_FILES}
        )
    else()
        add_executable(my-jobz
            ${PROJECT_SOURCES}
            ${PROJECT_HEADERS}
            ${PROJECT_FORMS}
            ${RESOURCES_FILES}
        )
    endif()
endif()
target_link_libraries(my-jobz PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql)
target_link_libraries(my-jobz PRIVATE ${SQLITE_DIR}/sqlite3.dll)
set_target_properties(my-jobz PROPERTIES OUTPUT_NAME MyJobz)


# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.my-jobz)
endif()
set_target_properties(my-jobz PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS my-jobz
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(my-jobz)
endif()
