import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtWebEngine

import jayrickaby.lj_launcher.authentication 1.0
import jayrickaby.lj_launcher.application 1.0
import jayrickaby.lj_launcher.downloader 1.0
import jayrickaby.lj_launcher.launcher 1.0

import "./template"

LauncherPage {
    id: control

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

        // Site
        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: bar.currentIndex

            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true

                WebEngineView {
                    id: newsSite
                    anchors.fill: parent

                    url: Qt.resolvedUrl("https://jayrickaby.github.io/lj-launcher/")

                    onJavaScriptConsoleMessage: function(level, message, lineNumber, sourceID) {
                        console.log("[Web Console]: " + message);
                    }
                }
            }
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "blue"
            }
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "green"
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

                           model: Launcher.profile_names

                           onCurrentIndexChanged: {
                               Launcher.current_profile = currentIndex;
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
                        }
                        Button {
                            Layout.preferredWidth: 85
                            Layout.preferredHeight: 21

                            text: qsTr("Edit Profile")
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
                        text: qsTr(Launcher.user_message)
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
}