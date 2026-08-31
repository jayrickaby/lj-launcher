import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import jayrickaby.lj_launcher

Item {
    id: root

    readonly property bool authenticated: Authentication.authenticated

    readonly property int gameState: Game.state
    readonly property bool gameUninitialised: Game.state === Game.GameState.UNINITIALISED

    ColumnLayout {
        anchors.fill: parent
        spacing: 2

        Text {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            Layout.topMargin: -4

            horizontalAlignment: Text.AlignHCenter
            text: qsTr(Launcher.userMessage)
        }

        Button {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
            Layout.preferredWidth: 87
            Layout.preferredHeight: 21

            text: qsTr("Switch User")

            enabled: authenticated && gameUninitialised

            onClicked: Authentication.logOut();
        }
    }
}
