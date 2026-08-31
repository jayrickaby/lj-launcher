import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import jayrickaby.lj_launcher

import "./CommandBar"
import "./LauncherPages"

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
    property bool gameLaunching: Game.state === Game.GameState.LAUNCHING

    property alias profileEditor: profileEditorLoader

    Loader {
        id: profileEditorLoader
        active: false
        sourceComponent: ProfileEditor{}
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
            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true

                // TODO: Get rid of bounce

                TextArea {
                    font.family: Launcher.monospaceFont
                    font.pointSize: 12

                    readOnly: true
                    wrapMode: TextArea.Wrap

                    text: Launcher.logs.join("\n")
                }
            }

            // Profile Page
            ProfilesPage {
                Layout.fillWidth: true
                Layout.fillHeight: true

                columnWidths: [0.5, 0.5]

                editor: profileEditor
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

                ProfileSelector {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    editor: profileEditorLoader
                }


                // spacer
                Item {
                    Layout.fillWidth: true
                }

                // Play
                Button {
                    id: playButton
                    Layout.alignment: Qt.AlignHCenter
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    font.bold: true

                    text: {
                        if (!authenticated || !versionsGotten) return qsTr("Loading...");
                        if (gamePreparing) return qsTr("Preparing...");
                        if (gameLaunching) return qsTr("Launching...");
                        if (!gameUninitialised) return qsTr("Installing...");

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


                // spacer
                Item {
                    Layout.fillWidth: true
                }

                // User
                ColumnLayout {
                    Layout.alignment: Qt.AlignRight
                    Layout.fillHeight: true
                    Layout.fillWidth: true

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

                        // TODO: Disable this and others when launching
                        text: qsTr("Switch User")

                        enabled: authenticated

                        onClicked: Authentication.logOut();
                    }
                }
            }
        }
    }
}