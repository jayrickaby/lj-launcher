import QtQuick
import QtQuick.Controls

Menu {
    id: root
    property var editor
    property var targetUuid: null

    readonly property int gameState: Game.state
    readonly property bool gameUninitialised: Game.state === Game.GameState.UNINITIALISED

    signal deleteActionPrompted()

    MenuItem {
        action: NewProfileAction {
            editor: root.editor
            profileId: targetUuid
        }

        enabled: gameUninitialised
    }
    MenuItem {
        action: CopyProfileAction {
            profileId: targetUuid
        }

        enabled: gameUninitialised
    }
    MenuItem {
        text: "Delete Profile"
        onTriggered: root.deleteActionPrompted()

        enabled: gameUninitialised
    }
    MenuItem {
        action: OpenGameDirectoryAction {
            profileId: targetUuid
        }
    }
}