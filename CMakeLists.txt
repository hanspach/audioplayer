cmake_minimum_required(VERSION 3.16)

project(audioplayer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Quick QuickControls2 QuickDialogs2 Multimedia LinguistTools)

qt_standard_project_setup(REQUIRES 6.5)

set(RESOURCE_FILES main.qrc)
QT6_ADD_RESOURCES(PROJECT_RESOURCES ${RESOURCE_FILES})

qt_add_executable(appaudioplayer
    main.cpp  ${PROJECT_RESOURCES}
)

qt_add_qml_module(appaudioplayer
    URI audioplayer
    VERSION 1.0
    QML_FILES Main.qml
    SOURCES initvaluesmodel.h initvaluesmodel.cpp
    SOURCES stationlistmodel.h stationlistmodel.cpp
    SOURCES filelistmodel.h filelistmodel.cpp
    QML_FILES RadioView.qml
    QML_FILES FileView.qml
    QML_FILES SettingsView.qml
    QML_FILES MsgToolButton.qml
    QML_FILES BigKnobSlider.qml
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appaudioplayer PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appaudioplayer
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(ANDROID)
    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
     if (ANDROID_ABI STREQUAL "armeabi-v7a")
        set(APPLICATION_LIBS_BIN
            ${CMAKE_CURRENT_SOURCE_DIR}/lib/android_armv7/libplugins_multimedia_androidmediaplugin_armeabi-v7a.so
            ${CMAKE_CURRENT_SOURCE_DIR}/lib/android_armv7/libplugins_multimedia_ffmpegmediaplugin_armeabi-v7a.so
            ${CMAKE_CURRENT_SOURCE_DIR}/lib/android_armv7/libcrypto_3.so
            ${CMAKE_CURRENT_SOURCE_DIR}/lib/android_armv7/libssl_3.so
        )
        set_target_properties(appaudioplayer
            PROPERTIES QT_ANDROID_EXTRA_LIBS "${APPLICATION_LIBS_BIN}"
        )
    endif()
endif()

target_link_libraries(appaudioplayer
    PRIVATE Qt6::Quick Qt6::QuickControls2 Qt6::QuickDialogs2 Qt6::Core Qt6::Network
)

qt_add_translations(appaudioplayer TS_FILES
    translations/i18n_de.ts
    translations/i18n_en.ts
)

include(GNUInstallDirs)
install(TARGETS appaudioplayer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
