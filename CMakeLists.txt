cmake_minimum_required(VERSION 3.16)

project(lj_launcher VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/dist" CACHE PATH "Default install path" FORCE)


find_package(Qt6 REQUIRED COMPONENTS
        Qml
        Quick
        QuickControls2
        WebEngineQuick
)

qt_standard_project_setup(REQUIRES 6.10)

qt_add_executable(lj_launcher
    src/main.cc
        src/Application.cc
        src/Application.h
        src/Launcher.cc
        src/Launcher.h
        src/Profiles.cc
        src/Profiles.h
        src/net/Versions.cc
        src/net/Versions.h
        src/net/Authentication.cc
        src/net/Authentication.h
        src/net/NetworkRequester.cc
        src/net/NetworkRequester.h
        src/net/Downloader.cc
        src/net/Downloader.h
        src/minecraft/assets/AssetIndex.cc
        src/minecraft/assets/AssetIndex.h
        src/sys/io/JsonUtils.cc
        src/sys/io/JsonUtils.h
        src/minecraft/lib/Library.h
        src/sys/info/OperatingSystem.h
        src/minecraft/lib/LibraryIndex.cc
        src/minecraft/lib/LibraryIndex.h
        src/sys/info/SystemInfo.cc
        src/sys/info/SystemInfo.h
        src/minecraft/ver/VersionManifest.cc
        src/minecraft/ver/VersionManifest.h
        src/sys/io/FileSystem.cc
        src/sys/io/FileSystem.h
        src/minecraft/ver/ClientJson.cc
        src/minecraft/ver/ClientJson.h
        src/minecraft/exec/Game.cc
        src/minecraft/exec/Game.h
        src/net/DownloadItem.cc
        src/net/DownloadItem.h
        src/minecraft/profiles/ProfilesTable.cc
        src/minecraft/profiles/ProfilesTable.h
        src/minecraft/profiles/ProfileManager.cc
        src/minecraft/profiles/ProfileManager.h
        src/minecraft/profiles/ProfileEntry.cc
        src/minecraft/profiles/ProfileEntry.h
        src/minecraft/ver/Argument.h
        src/minecraft/ver/Rule.h
        src/minecraft/ver/Rule.cc
        src/minecraft/exec/JavaVirtualMachine.cc
        src/minecraft/exec/JavaVirtualMachine.h
)

qt_add_qml_module(lj_launcher
    URI jayrickaby.lj_launcher
    RESOURCE_PREFIX /
    QML_FILES
        qml/Main.qml
        qml/LauncherMenu.qml
        qml/LoginForm.qml
        qml/ProfileEditor.qml
        qml/Actions/CopyProfileAction.qml
        qml/Actions/DeleteProfileAction.qml
        qml/Actions/EditProfileAction.qml
        qml/Actions/NewProfileAction.qml
        qml/Actions/OpenGameDirectoryAction.qml
        qml/LauncherPages/NewsPage.qml
        qml/LauncherPages/ProfilesPage.qml
        qml/LauncherPages/template/ProfileCellDelegate.qml
        qml/LauncherPages/template/ProfilesTableMenu.qml
        qml/ProfileEditor/CheckBoxPopup.qml
    RESOURCES
        assets/dirt.png
        assets/minecraft_logo.png
        assets/icons/icon.png
)

target_include_directories(lj_launcher PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/minecraft/profiles
)

        # Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(lj_launcher PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.lj_launcher
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
#    WIN32_EXECUTABLE TRUE
)

target_link_libraries(lj_launcher
    PRIVATE Qt6::Qml
    PRIVATE Qt6::Quick
    PRIVATE Qt6::QuickControls2
    PRIVATE Qt6::WebEngineQuick
)

install(TARGETS lj_launcher
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_qml_app_script(
    TARGET lj_launcher
    OUTPUT_SCRIPT deploy_script
    NO_TRANSLATIONS
)
install(SCRIPT ${deploy_script})