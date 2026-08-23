import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Fusion
import QtQuick.Layouts

import jayrickaby.lj_launcher

import "./LauncherPages"
import "./Actions"

Item {
    id: control

    property int authState: Authentication.authState
    property bool authenticated: authState === Authentication.AuthState.AUTHENTICATED

    property int versionManifestState: VersionManifest.manifestState
    property bool versionsGotten: versionManifestState === VersionManifest.ManifestState.PRESENT

    property int gameState: Game.state
    property bool gameUninitialised: Game.state === Game.GameState.UNINITIALISED
    property bool gamePreparing: Game.state === Game.GameState.PREPARING
    property bool gameDownloading: Game.state === Game.GameState.DOWNLOADING
    property bool gameDownloaded: Game.state === Game.GameState.DOWNLOADED

    Loader {
        id: profileEditorLoader
        source: "ProfileEditor.qml"
        active: false

        onLoaded: {
            item.show();
            item.closing.connect(function() {
                profileEditorLoader.active = false;
            });
        }
    }


    NewProfileAction {
        id: newProfileAction

        editorLoader: profileEditorLoader
        profileId: profileChooser.currentValue
    }
    EditProfileAction {
        id: editProfileAction

        editorLoader: profileEditorLoader
        profileId: profileChooser.currentValue
    }

    ColumnLayout {
        anchors.fill: parent

        // Tabs
        TabBar {
            id: bar

            Layout.fillWidth: true
            // Layout.fillHeight: true

            TabButton {
                text: qsTr("Update Notes")
                width: implicitWidth
            }
            TabButton {
                text: qsTr("Launcher Log")
                width: implicitWidth
            }
            TabButton {
                text: qsTr("Profile Editor")
                width: implicitWidth
            }
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: bar.currentIndex


            // Update Notes
            NewsPage {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            // Launcher Log
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "blue"
            }

            // Profile Editor
            ProfilesPage {
                Layout.fillWidth: true
                Layout.fillHeight: true

                columnWidths: [0.5, 0.5]
                model: ProfilesTable {}
            }
        }

        // Progress Bar
        ProgressBar {
            z: 0
            Layout.fillWidth: true
            Layout.preferredHeight: 16

            visible: gameDownloading

            value: Downloader.currentProgress
            to: Downloader.currentProgressMax

            Text {
                z: 1
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter

                text: Downloader.currentFile
            }
        }

        // Command Bar
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 60

            Layout.alignment: Qt.AlignBottom

            RowLayout {
                spacing: 0
                anchors.fill: parent
                anchors.leftMargin: 4
                anchors.topMargin: 5
                anchors.rightMargin: 4
                anchors.bottomMargin: 5
                uniformCellSizes: true

                // Profile
                ColumnLayout {
                    Layout.alignment: Qt.AlignLeft
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    // Combobox
                    RowLayout {
                       Text { text: qsTr("Profile:") }
                       ComboBox {
                           id: profileChooser
                           Layout.preferredWidth: 137
                           Layout.preferredHeight: 20

                           valueRole: "id"
                           textRole: "name"

                           model: authenticated && versionsGotten ? Profiles.profiles
                                                : [{ "name": "Loading profiles...", "id": "" }]

                           enabled: authenticated && versionsGotten

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
                        spacing: 2

                        Button {
                            Layout.preferredWidth: 85
                            Layout.preferredHeight: 21

                            action: newProfileAction

                            enabled: authenticated && versionsGotten
                        }
                        Button {
                            Layout.preferredWidth: 85
                            Layout.preferredHeight: 21

                            action: editProfileAction

                            enabled: authenticated && versionsGotten
                        }
                    }
                }

                // Play
                Button {
                    id: playButton
                    Layout.alignment: Qt.AlignHCenter
                    Layout.fillHeight: true
                    Layout.preferredWidth: 290

                    font.bold: true

                    text: {
                        if (!authenticated || !versionsGotten) return qsTr("Loading...");
                        if (gamePreparing) return qsTr("Preparing...");
                        if (!gameUninitialised) return qsTr("Downloading...");

                        return qsTr("Play");
                    }

                    enabled: {
                        if (!authenticated || !versionsGotten || !gameUninitialised) return false;
                        return true;
                    }

                    onClicked: {
                        Launcher.play();
                    }
                }

                // User
                ColumnLayout {
                    Layout.alignment: Qt.AlignRight
                    Layout.fillHeight: true

                    spacing: 2

                    Text {
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                        Layout.topMargin: -4

                        horizontalAlignment: Text.AlignHCenter
                        text: qsTr(Launcher.userMessage)
                    }

                    Button {
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
                        Layout.preferredWidth: 87
                        Layout.preferredHeight: 21

                        text: qsTr("Switch User")

                        enabled: authenticated
                    }
                }
            }
        }
    }
}