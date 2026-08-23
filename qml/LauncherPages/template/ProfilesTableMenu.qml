import QtQuick
import QtQuick.Controls

Menu {
    id: root
    property var editorLoader
    property var targetUuid: null

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
        action: DeleteProfileAction {
            profileId: targetUuid
        }
    }
    MenuItem {
        action: OpenGameDirectoryAction {
            profileId: targetUuid
        }
    }
}