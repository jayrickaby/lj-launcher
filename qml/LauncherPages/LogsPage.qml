import QtQuick
import QtQuick.Controls

import jayrickaby.lj_launcher

FocusScope {
    id: root

    ScrollView {
        // TODO: Get rid of bounce
        anchors.fill: parent

        TextArea {
            font.family: Launcher.monospaceFont
            font.pointSize: 12

            readOnly: true
            wrapMode: TextArea.Wrap

            text: Launcher.logs.join("\n")
        }
    }
}
