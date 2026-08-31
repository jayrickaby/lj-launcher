import QtQuick
import QtQuick.Controls
import QtWebEngine

FocusScope {
    id: root

    ScrollView {
        anchors.fill: parent

        WebEngineView {
            id: newsSite
            anchors.fill: parent

            url: Qt.resolvedUrl("https://jayrickaby.github.io/lj-launcher/")
        }
    }
}
