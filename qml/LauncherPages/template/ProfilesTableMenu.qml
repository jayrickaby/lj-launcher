import QtQuick
import QtQuick.Controls

Menu {
    id: root
    property var editor
    property var targetUuid: null

    signal deleteActionPrompted()

    MenuItem {
        action: NewProfileAction {
            editor: root.editor
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