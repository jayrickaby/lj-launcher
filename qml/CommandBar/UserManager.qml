import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import jayrickaby.lj_launcher

Item {
    id: root

    property bool authenticated: Authentication.authenticated

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

            // TODO: Disable this and others when launching
            text: qsTr("Switch User")

            enabled: authenticated

            onClicked: Authentication.logOut();
        }
    }
}
