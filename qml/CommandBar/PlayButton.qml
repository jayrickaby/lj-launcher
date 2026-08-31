import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import jayrickaby.lj_launcher

Button {
    id: root

    font.bold: true

    readonly property bool authenticated: Authentication.authenticated
    readonly property bool versionsGotten: VersionManifest.present

    readonly property int gameState: Game.state
    readonly property bool gameUninitialised: Game.state === Game.GameState.UNINITIALISED
    readonly property bool gamePreparing: Game.state === Game.GameState.PREPARING
    readonly property bool gameLaunching: Game.state === Game.GameState.LAUNCHING

    text: {
        if (!authenticated || !versionsGotten) return qsTr("Loading...");
        if (gamePreparing) return qsTr("Preparing...");
        if (gameLaunching) return qsTr("Launching...");
        if (!gameUninitialised) return qsTr("Installing...");

        return qsTr("Play");
    }

    enabled: {
        if (!authenticated || !versionsGotten || !gameUninitialised) return false;
        return true;
    }

    onClicked: {
        Launcher.play();
    }
}