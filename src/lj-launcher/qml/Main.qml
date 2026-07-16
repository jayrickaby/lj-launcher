import QtQuick
import QtQuick.Controls

import jayrickaby.lj_launcher.application 1.0
import jayrickaby.lj_launcher.auth 1.0

import "./pages"

ApplicationWindow {
    id: root

    width: 886
    height: 544

    title: Application.defaultTitle

    visible: true

        Connections {
        target: Auth

        function onAuthenticated(status) {
            print(status)
            if (status) {
                pageLoader.source = "./pages/Launcher.qml"
            }
        }
    }

    Loader {
        id: pageLoader
        anchors.fill: parent

        source: "./pages/LoginForm.qml"
    }

    Component.onCompleted: {
        Auth.tryStoredRefresh()
    }
}