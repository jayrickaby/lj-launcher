import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import jayrickaby.lj_launcher

import "./CommandBar"
import "./LauncherPages"

Item {
    id: control

    readonly property int authState: Authentication.authState
    readonly property bool authenticated: Authentication.authenticated

    readonly property int versionManifestState: VersionManifest.manifestState
    readonly property bool versionsGotten: VersionManifest.present

    readonly property int gameState: Game.state
    readonly property bool gameDownloading: Game.state === Game.GameState.DOWNLOADING

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
            LogsPage {
                Layout.fillWidth: true
                Layout.fillHeight: true
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
        Item {
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

                ProfileSelector {
                    Layout.alignment: Qt.AlignLeft
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.horizontalStretchFactor: 2

                    editor: profileEditorLoader
                }

                // spacer
                Item {
                    Layout.fillWidth: true
                    Layout.horizontalStretchFactor: 1
                }

                PlayButton {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.horizontalStretchFactor: 5
                }


                // spacer
                Item {
                    Layout.fillWidth: true
                    Layout.horizontalStretchFactor: 1
                }

                // User
                UserManager {
                    Layout.alignment: Qt.AlignRight
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.horizontalStretchFactor: 2
                }

            }
        }
    }
}