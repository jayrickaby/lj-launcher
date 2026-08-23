import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQml.Models

// Adapted from plasma-systemmonitor/src/table/BaseCellDelegate.qml and TextCellDelegate.qml
TableViewDelegate {
    id: root

    property var contextMenu: null

    text: model.display

    // Important: Don't remove this until QTBUG-84858 is resolved properly.
    Accessible.role: Accessible.Cell

    leftInset: 0
    bottomInset: 0
    rightInset: 0
    topInset: 0

    contentItem: Text {
        text: root.text
        maximumLineCount: 1
        elide: Text.ElideRight

        color: selected ? "white" : "black"
    }

    background: Rectangle {
        border.width: 1
        color: selected ? palette.highlight : "white"
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton

        onClicked: {
            if (contextMenu) {
                contextMenu.targetUuid = model.uuid
                contextMenu.popup()
            }
        }
    }
}