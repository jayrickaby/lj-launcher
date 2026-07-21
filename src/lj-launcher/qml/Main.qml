import QtQuick
import QtQuick.Controls

import jayrickaby.lj_launcher.application 1.0
import jayrickaby.lj_launcher.authentication 1.0

import "./pages"

ApplicationWindow {
    id: root

    width: 886
    height: 544

    title: Application.default_title

    visible: true

    Connections {
        target: Authentication

        function onAuthenticated(authenticated) {
            if (authenticated) {
                pageLoader.source = "./pages/Launcher.qml";
                return;
            }

            pageLoader.source = "./pages/LoginForm.qml";
        }
    }

    Loader {
        id: pageLoader
        anchors.fill: parent

        source: "./pages/Launcher.qml"
    }

    Component.onCompleted: {
        Authentication.try_stored_refresh();
    }
}