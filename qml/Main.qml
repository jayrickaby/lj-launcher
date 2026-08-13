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

    property int authState: Authentication.authState

    Loader {
        id: pageLoader
        anchors.fill: parent

        source: authState === Authentication.AuthState.AUTHENTICATED
            || authState === Authentication.AuthState.AUTHENTICATING_REFRESH ? "./pages/LauncherPage.qml" : "./pages/LoginForm.qml"
    }

    Component.onCompleted: {
        // Authentication.try_stored_refresh();
    }
}