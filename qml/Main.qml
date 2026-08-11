import QtQuick
import QtQuick.Controls

import jayrickaby.lj_launcher

import "./pages"

ApplicationWindow {
    id: root

    width: 900
    height: 580

    title: Application.defaultTitle

    visible: true

    Loader {
        id: pageLoader
        anchors.fill: parent

        source: Authentication.authenticated || Authentication.has_stored_refresh() ? "./pages/Launcher.qml" : "./pages/LoginForm.qml"
    }

    Component.onCompleted: {
        Authentication.try_stored_refresh();
    }
}