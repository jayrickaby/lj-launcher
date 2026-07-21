import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtWebEngine

import jayrickaby.lj_launcher.authentication 1.0
import jayrickaby.lj_launcher.application 1.0

import "./template"

LauncherPage {
    id: control

    Connections {
        target: Authentication

        function onAuthenticated() {
            playButton.text = qsTr("Play");
            playButton.enabled = true;
        }
    }

    ColumnLayout {
        anchors.fill: parent

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

                    text: qsTr("Loading")
                    enabled: false
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
                        text: qsTr("Welcome, guest! Please log in.\nReady to download & play Minecraft 26.2")
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