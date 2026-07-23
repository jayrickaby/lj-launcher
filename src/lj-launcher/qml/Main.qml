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

    Loader {
        id: pageLoader
        anchors.fill: parent

        source: Authentication.authenticated || Authentication.has_stored_refresh ? "./pages/Launcher.qml" : "./pages/LoginForm.qml"
    }

    Component.onCompleted: {
        Authentication.try_stored_refresh();
    }
}