import QtQuick

Item {
    id: control

    property Item background: null

    onBackgroundChanged: {
        if (background) {
            background.parent = control
            background.z = -1
            background.anchors.fill = control
        }
    }
}