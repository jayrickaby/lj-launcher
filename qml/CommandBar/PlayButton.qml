import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import jayrickaby.lj_launcher

Button {
    id: root

    font.bold: true

    property bool authenticated: Authentication.authenticated
    property bool versionsGotten: VersionManifest.present

    property int gameState: Game.state
    property bool gameUninitialised: Game.state === Game.GameState.UNINITIALISED
    property bool gamePreparing: Game.state === Game.GameState.PREPARING
    property bool gameDownloading: Game.state === Game.GameState.DOWNLOADING
    property bool gameDownloaded: Game.state === Game.GameState.DOWNLOADED
    property bool gameLaunching: Game.state === Game.GameState.LAUNCHING

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