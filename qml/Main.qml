import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

import jayrickaby.lj_launcher

ApplicationWindow {
    id: root

    width: 900
    height: 580

    title: `${Application.defaultTitle} ${Application.version}`

    visible: true

    property int authState: Authentication.authState

    Loader {
        id: pageLoader
        anchors.fill: parent

        visible: Launcher.javaExecutable

        source: authState === Authentication.AuthState.AUTHENTICATED
            || authState === Authentication.AuthState.AUTHENTICATING_REFRESH
            ? "./LauncherMenu.qml" : "./LoginForm.qml"
    }

    Component.onCompleted: {
        if (Launcher.javaExecutable) {
            Authentication.tryStoredRefreshToken();
        }
        else {
            javaWarning.open();
        }
    }

    MessageDialog {
        id: javaWarning

        buttons: MessageDialog.Ok
        text: "The system could not find Java! Please install it and make sure it is in PATH"

        onAccepted: {
            root.close();
        }
    }
}