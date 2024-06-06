cmake_minimum_required(VERSION 3.5)

project(Arma3ModLauncher VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

set(app_icon_resource_windows "${CMAKE_CURRENT_SOURCE_DIR}/Arma3ModLauncher.rc")

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Arma3ModLauncher
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        optionstab.h optionstab.cpp
        util.h util.cpp
        settings.h settings.cpp
        modstab.h modstab.cpp
        modgroupstreewidgetitem.h modgroupstreewidgetitem.cpp
        modgroupstreewidget.h modgroupstreewidget.cpp
        availablemodstreewidget.h availablemodstreewidget.cpp
        modstabtreewidget.h modstabtreewidget.cpp
        modstabdragdropdata.h modstabdragdropdata.cpp
        launcher.h launcher.cpp
        ${app_icon_resource_windows}
        Arma3ModLauncher.rc
        README.md
        images/image1.jpg images/image2.jpg .gitignore credits.txt icon.ico
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Arma3ModLauncher APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Arma3ModLauncher SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Arma3ModLauncher
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Arma3ModLauncher PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Arma3ModLauncher)
endif()
set_target_properties(Arma3ModLauncher PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Arma3ModLauncher
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Arma3ModLauncher)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
