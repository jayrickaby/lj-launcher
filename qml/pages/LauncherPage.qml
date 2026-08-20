import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Fusion
import QtQuick.Layouts
import QtWebEngine

import jayrickaby.lj_launcher

import "./template"

Item {
    id: control

    property int authState: Authentication.authState
    property bool authenticated: authState === Authentication.AuthState.AUTHENTICATED

    property int versionManifestState: Versions.manifestState
    property bool versionsGotten: versionManifestState === Versions.ManifestState.PRESENT

    Loader {
        id: profileEditorLoader
        source: "./ProfileEditor.qml"
        active: false

        onLoaded: {
            item.show();
            item.closing.connect(function() {
                profileEditorLoader.active = false;
            });
        }
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
            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true

                WebEngineView {
                    id: newsSite
                    anchors.fill: parent

                    url: Qt.resolvedUrl("https://jayrickaby.github.io/lj-launcher/")

                }
            }

            // Launcher Log
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "blue"
            }

            // Profile Editor
            ProfileEditorPage {
                Layout.fillWidth: true
                Layout.fillHeight: true

                columnWidths: [0.5, 0.5]
                model: Profiles.model
            }
        }

        // Progress Bar
        ProgressBar {
            z: 0
            Layout.fillWidth: true
            Layout.preferredHeight: 16

            visible: Downloader.downloading

            value: Downloader.download_progress
            to: Downloader.download_progress_max

            Text {
                z: 1
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter

                text: Downloader.download_status
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

                            text: qsTr("New Profile")

                            enabled: authenticated && versionsGotten

                            onClicked: {
                                profileEditorLoader.active = true;
                                let profile = Profiles.currentProfile;
                                profile["name"] = `Copy of ${profile["name"]}`;
                                profileEditorLoader.item.setProfile(profile, null);
                            }
                        }
                        Button {
                            Layout.preferredWidth: 85
                            Layout.preferredHeight: 21

                            text: qsTr("Edit Profile")

                            enabled: authenticated && versionsGotten

                            onClicked: {
                                profileEditorLoader.active = true;
                                let profile = Profiles.currentProfile;
                                let profileId = Profiles.currentProfileId;
                                profileEditorLoader.item.setProfile(profile, profileId);
                            }
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

                    property bool preparing: false

                    text: {
                        if (!authenticated || !versionsGotten) return qsTr("Loading...");
                        if (Downloader.downloading) return qsTr("Downloading...");
                        if (preparing) return qsTr("Preparing...");

                        return qsTr("Play");
                    }

                    enabled: {
                        if (!authenticated || !versionsGotten) return false;
                        if (preparing) return false;

                        return true;
                    }

                    onClicked: {
                        preparing = true;
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

    ProfileEditor {
        id: profileEditor
    }
}