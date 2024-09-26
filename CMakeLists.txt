cmake_minimum_required(VERSION 3.16)

project(audioplayer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick QuickControls2 QuickDialogs2 Multimedia LinguistTools)

qt_standard_project_setup(REQUIRES 6.8)

set(RESOURCE_FILES main.qrc)
QT6_ADD_RESOURCES(PROJECT_RESOURCES ${RESOURCE_FILES})

qt_add_executable(sap
    main.cpp  ${PROJECT_RESOURCES}
)

qt_add_qml_module(sap
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

target_link_libraries(sap
    PRIVATE Qt6::Core Qt6::CorePrivate Qt6::Gui  Qt6::Quick Qt6::QuickControls2 Qt6::QuickDialogs2 Qt6::Network Qt6::Multimedia
)

if(UNIX AND NOT APPLE AND CMAKE_CROSSCOMPILING)
    find_package(Qt6 REQUIRED COMPONENTS QuickTemplates2)

    # Work around QTBUG-86533
    target_link_libraries(sap PRIVATE Qt::QuickTemplates2)
endif()

if(ANDROID)
    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
     if (ANDROID_ABI STREQUAL "armeabi-v7a")
        set(APPLICATION_LIBS_BIN
            ${CMAKE_CURRENT_SOURCE_DIR}/lib/android_armv7/libplugins_multimedia_androidmediaplugin_armeabi-v7a.so
            ${CMAKE_CURRENT_SOURCE_DIR}/lib/android_armv7/libplugins_multimedia_ffmpegmediaplugin_armeabi-v7a.so
            ${CMAKE_CURRENT_SOURCE_DIR}/lib/android_armv7/libcrypto_3.so
            ${CMAKE_CURRENT_SOURCE_DIR}/lib/android_armv7/libssl_3.so
        )
        set_target_properties(sap
            PROPERTIES QT_ANDROID_EXTRA_LIBS "${APPLICATION_LIBS_BIN}"
        )
    endif()
endif()

set_target_properties(sap PROPERTIES
     QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android"
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.sap
#    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
#    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
#    MACOSX_BUNDLE TRUE
#    WIN32_EXECUTABLE TRUE
)

qt_add_translations(sap TS_FILES
    translations/i18n_de.ts
    translations/i18n_en.ts
)

include(GNUInstallDirs)
install(TARGETS sap
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_qml_app_script(
    TARGET sap
    OUTPUT_SCRIPT deploy_script
    MACOS_BUNDLE_POST_BUILD
    NO_UNSUPPORTED_PLATFORM_ERROR
    DEPLOY_USER_QML_MODULES_ON_UNSUPPORTED_PLATFORM
)
install(SCRIPT ${deploy_script})


