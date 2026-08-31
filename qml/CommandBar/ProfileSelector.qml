import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import jayrickaby.lj_launcher

import "../Actions"

Item {
    id: root

    readonly property bool authenticated: Authentication.authenticated
    readonly property bool versionsGotten: VersionManifest.present

    readonly property int gameState: Game.state
    readonly property bool gameUninitialised: Game.state === Game.GameState.UNINITIALISED

    property var editor

    NewProfileAction {
        id: newProfileAction

        editor: root.editor
        profileId: profileChooser.currentValue
    }
    EditProfileAction {
        id: editProfileAction

        editor: root.editor
        profileId: profileChooser.currentValue
    }

    ColumnLayout {
        anchors.fill: parent
        uniformCellSizes: true

        // Combobox
        RowLayout {
            Text { text: qsTr("Profile:") }
            ComboBox {
                id: profileChooser
                Layout.fillWidth: true
                Layout.preferredHeight: 20

                valueRole: "id"
                textRole: "name"

                model: authenticated && versionsGotten ? Profiles.profiles
                    : [{ "name": "Loading profiles...", "id": "" }]

                enabled: authenticated && versionsGotten && gameUninitialised

                currentIndex: {
                    if (!versionsGotten) return 0;

                    var list = Profiles.profiles;
                    for (var i = 0; i < list.length; i++) {
                        if (list[i].id === Profiles.currentProfileId) return i;
                    }
                    return -1;
                }

                onActivated: {
                    Profiles.currentProfileId = currentValue;
                }
            }
        }


        // Buttons
        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            spacing: 2

            Button {
                Layout.fillWidth: true
                Layout.preferredHeight: 21

                action: newProfileAction

                enabled: authenticated && versionsGotten && gameUninitialised
            }
            Button {
                Layout.fillWidth: true
                Layout.preferredHeight: 21

                action: editProfileAction

                enabled: authenticated && versionsGotten && gameUninitialised
            }
        }
    }
}