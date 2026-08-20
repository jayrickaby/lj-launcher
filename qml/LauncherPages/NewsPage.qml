import QtQuick
import QtQuick.Controls
import QtWebEngine

ScrollView {
    WebEngineView {
        id: newsSite
        anchors.fill: parent

        url: Qt.resolvedUrl("https://jayrickaby.github.io/lj-launcher/")

    }
}