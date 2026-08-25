import QtQuick
import QtQuick.Controls

Menu {
    id: root
    property var editorLoader
    property var targetUuid: null

    signal deleteActionPrompted()

    MenuItem {
        action: NewProfileAction {
            editorLoader: root.editorLoader
            profileId: targetUuid
        }
    }
    MenuItem {
        action: CopyProfileAction {
            profileId: targetUuid
        }
    }
    MenuItem {
        text: "Delete Profile"
        onTriggered: root.deleteActionPrompted()
    }
    MenuItem {
        action: OpenGameDirectoryAction {
            profileId: targetUuid
        }
    }
}