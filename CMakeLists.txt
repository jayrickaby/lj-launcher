cmake_minimum_required(VERSION 3.16)

project(lj_launcher VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.10)

qt_add_executable(lj_launcher
    src/main.cc
        src/Application.cc
        src/Application.h
        src/Authentication.cc
        src/Authentication.h
)

qt_add_qml_module(lj_launcher
    URI jayrickaby.lj_launcher
    QML_FILES
        qml/Main.qml
        qml/pages/Launcher.qml
        qml/pages/LoginForm.qml
        qml/pages/ProfileEditor.qml
        qml/pages/template/LauncherPage.qml
        qml/pages/template/ProfileCellDelegate.qml
        qml/pages/template/ProfileEditorPage.qml
    RESOURCES
        assets/dirt.png
        assets/minecraft_logo.png
        assets/icons/icon.png
)

target_include_directories(lj_launcher PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

        # Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(lj_launcher PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.lj_launcher
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(lj_launcher
    PRIVATE Qt6::Quick
)

install(TARGETS lj_launcher
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
