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

        function onAuthenticated(status) {
            if (status) {
                pageLoader.source = "./pages/Launcher.qml";
            }
        }
    }

    Loader {
        id: pageLoader
        anchors.fill: parent

        source: "./pages/LoginForm.qml"
    }

    Component.onCompleted: {
        Authentication.try_stored_refresh();
    }
}