import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtWebEngine

import jayrickaby.lj_launcher.auth 1.0
import jayrickaby.lj_launcher.application 1.0

import "./template"

LauncherPage {
    id: control

    Rectangle {
        height: 200
        width: 200
        Text {
            text: "Miaow"
        }
    }

    background: Image {
        sourceSize.width: 64
        sourceSize.height: 64

        fillMode: Image.Tile

        // fillMode: Image.TileVertically
        source: Qt.resolvedUrl(Application.parentPath + "/qml/assets/dirt.png")
    }
}