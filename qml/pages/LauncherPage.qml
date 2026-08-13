import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Fusion
import QtQuick.Layouts
import QtWebEngine

import jayrickaby.lj_launcher

import "./template"

LauncherPage {
    id: control

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

                           model: Authentication.authenticated ? Profiles.profile_list : [{ "name": "Loading
                            profiles...", "id": "" }]
                           enabled: Authentication.authenticated

                           currentIndex: {
                               if (!Authentication.authenticated) return 0;

                               var list = Profiles.profile_list;
                               for (var i = 0; i < list.length; i++) {
                                   if (list[i].id === Profiles.current_id) return i;
                               }
                               return -1;
                           }

                           onActivated: {
                               Profiles.current_profile = currentValue;
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

                            onClicked: {
                                Profiles.set_mode("new");
                                profileEditorLoader.active = true;
                            }
                        }
                        Button {
                            Layout.preferredWidth: 85
                            Layout.preferredHeight: 21

                            text: qsTr("Edit Profile")

                            onClicked: {
                                Profiles.set_mode("edit");
                                profileEditorLoader.active = true;
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
                        if (!Authentication.authenticated) return qsTr("Loading...");
                        if (Downloader.downloading) return qsTr("Downloading...");
                        if (preparing) return qsTr("Preparing...");

                        return qsTr("Play");
                    }

                    enabled: {
                        if (!Authentication.authenticated) return false;
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
                    }
                }
            }
        }
    }

    ProfileEditor {
        id: profileEditor
    }
}