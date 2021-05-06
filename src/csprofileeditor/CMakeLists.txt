set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(${PROJECT_NAME}
    AllowedNamesItemDelegate.cpp
    CsLibUpdater.cpp
    EtcCsPersEditBridge.cpp
    ExportDialog.cpp
    main.cpp
    MainWindow.cpp
    MediaSelectorDialog.cpp
    ParameterTableModel.cpp
    ParameterTypeItemDelegate.cpp
    PersonalityEditDialog.cpp
    PersonalityTableModel.cpp
    PushButtonItemDelegate.cpp
    RangesEditDialog.cpp
    RangesTableModel.cpp
    SettingsDialog.cpp
    )
target_compile_definitions(${PROJECT_NAME} PUBLIC -DQT_NO_KEYWORDS)

if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    find_package(BreezeIcons REQUIRED)
    target_sources(${PROJECT_NAME} PRIVATE ${BreezeIcons_RES_DIRS}/breeze-icons.qrc)
endif ()

find_package(QSettingsContainer REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE
    csprofile
    cslibs
    QSettingsContainer::QSettingsContainer
    Qt5::Widgets
    )

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_GUI_IDENTIFIER "${PROJECT_ORGANIZATION_DOMAIN}.${PROJECT_NAME}"
    #    MACOSX_BUNDLE_ICON_FILE icon.icns
    MACOSX_BUNDLE_LONG_VERSION_STRING ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION})

# Qt packaging
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_options(${PROJECT_NAME} PRIVATE "/NODEFAULTLIB:library")
    # Bundle Qt runtime
    if (BUILD_PACKAGE)
        include("${PROJECT_SOURCE_DIR}/cmake/Windeployqt.cmake")
        windeployqt(${PROJECT_NAME} bin)
    endif ()
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    file(COPY "${PROJECT_SOURCE_DIR}/resources/app-icon.icns" DESTINATION "${PROJECT_BINARY_DIR}/meta")
    file(RENAME "${PROJECT_BINARY_DIR}/meta/app-icon.icns" "${PROJECT_BINARY_DIR}/meta/${PROJECT_NAME}.icns")
    target_sources(${PROJECT_NAME} PRIVATE "${PROJECT_BINARY_DIR}/meta/${PROJECT_NAME}.icns")
    set_source_files_properties("${PROJECT_BINARY_DIR}/meta/${PROJECT_NAME}.icns" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    if (BUILD_PACKAGE)
        find_program(MACDEPLOYQT_PROG macdeployqt)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMENT "Running macdeployqt"
            COMMAND ${MACDEPLOYQT_PROG} ARGS $<TARGET_BUNDLE_DIR:${PROJECT_NAME}> -verbose=1)
    endif ()
endif ()
