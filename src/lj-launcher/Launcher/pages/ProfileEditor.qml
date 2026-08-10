import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import jayrickaby.lj_launcher.launcher 1.0
import jayrickaby.lj_launcher.profiles 1.0
import "./template"

ApplicationWindow {
    id: control

    modality: Qt.ApplicationModal

    title: qsTr("Profile Editor")

    property var currentProfile: Profiles.current_profile

    leftPadding: 8; topPadding: 8; rightPadding: 8; bottomPadding: 8;

    ColumnLayout {
        id: content
        anchors.fill: parent

        // Profile Info
        GroupBox {
            title: qsTr("Profile Info")

            Layout.fillWidth: true
            // Layout.topMargin: -24

            GridLayout {
                anchors.fill: parent
                columns: 2

                rowSpacing: 4

                // Row 1
                Text {
                    // TODO: If (Default) change to username on auth.
                    text: qsTr("Profile Name:")

                    font.pointSize: 8
                }
                TextField {
                    id: profileName
                    Layout.fillWidth: true
                    Layout.preferredHeight: 21

                    text: qsTr(currentProfile.name)
                }

                // Row 2
                CheckBox {
                    id: checkGameDir

                    checked: !!currentProfile.gameDir

                    text: qsTr("Game Directory:")
                    font.pointSize: 8
                }
                TextField {
                    id: gameDir
                    Layout.fillWidth: true
                    Layout.preferredHeight: 21

                    text: {
                        var dir = currentProfile.gameDir || Launcher.game_directory;
                        return qsTr(dir.toString());
                    }

                    enabled: checkGameDir.checked
                }

                // Row 3
                CheckBox {
                    id: checkResolution

                    checked: !!currentProfile.resolution

                    text: qsTr("Resolution:")
                    font.pointSize: 8
                }
                RowLayout {
                    Layout.fillWidth: true

                    TextField {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 21

                        text: {
                            var res = currentProfile.resolution || Profiles.default_resolution;
                            return res.width;
                        }

                        enabled: checkResolution.checked
                    }
                    Text {
                        text: "x"

                        font.pointSize: 8
                    }
                    TextField {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 21

                        text: {
                            var res = currentProfile.resolution || Profiles.default_resolution;
                            return res.height;
                        }

                        enabled: checkResolution.checked
                    }
                }

                // Row 4
                CheckBox {
                    text: qsTr("Automatically ask Mojang for assistance with fixing crashes")

                    // TODO: Add actual crash stuff
                    checked: Launcher.settings.crashAssistance

                    font.pointSize: 8
                    Layout.columnSpan: 2

                    onCheckedChanged: Launcher.set_setting("crashAssistance", checked)
                }

                // Row 5
                CheckBox {
                    id: checkLauncherVisible
                    text: qsTr("Keep Launcher Open")

                    checked: Launcher.settings.keepLauncherOpen

                    font.pointSize: 8

                    onCheckedChanged: Launcher.set_setting("keepLauncherOpen", checked)
                }
                // CheckBox {
                //     id: checkLauncherVisible
                //     text: qsTr("Launcher Visibility:")
                //
                //     font.pointSize: 8
                // }
                // ComboBox {
                //     Layout.fillWidth: true
                //     Layout.preferredHeight: 21
                //
                //     // TODO: Find some way to have different states
                //     model: ["Hide launcher and re-open when game closes",
                //         "Close launcher when game starts",
                //         "Keep the launcher open"]
                //
                //     currentIndex: 1
                //
                //     enabled: checkLauncherVisible.checked
                // }
            }
        }

        // Version Selection
        GroupBox {
            title: qsTr("Version Selection")

            Layout.fillWidth: true

            GridLayout {
                anchors.fill: parent
                columns: 2

                rowSpacing: 4

                CheckBox{
                    Layout.columnSpan: 2
                    text: qsTr("Enable experimental development versions (\"snapshots\")")

                    checked: Launcher.settings.enableSnapshots

                    onCheckedChanged: Launcher.set_setting("enableSnapshots", checked)
                }
                // TODO: Find some way to store alpha and beta individually
                CheckBox{
                    Layout.columnSpan: 2
                    text: qsTr("Allow use of old \"Alpha\" and \"Beta\" Minecraft versions (From 2010-2011)")

                    checked: Launcher.settings.enableHistorical

                    onCheckedChanged: Launcher.set_setting("enableHistorical", checked)
                }
                //
                // CheckBox{
                //     text: qsTr("Allow use of old \"Beta\" Minecraft versions (From 2010-2011)")
                //
                //     checked: Launcher.settings.enableHistorical
                // }
                // CheckBox{
                //     text: qsTr("Allow use of old \"Alpha\" Minecraft versions (From 2010)")
                //
                //     checked: Launcher.settings.enableHistorical
                // }

                Text {
                    text: qsTr("Use version:");

                    font.pointSize: 8
                }
                ComboBox {
                    id: comboUseVersion
                    Layout.fillWidth: true
                    Layout.preferredHeight: 21
                    valueRole: "id"
                    textRole: "name"

                    model: Launcher.online_versions
                    currentValue: Profiles.current_version
                }
            }
        }

        // Java Settings
        GroupBox {
            title: qsTr("Java Settings (Advanced)")

            Layout.fillWidth: true

            GridLayout {
                anchors.fill: parent
                columns: 2

                rowSpacing: 4
                CheckBox{
                    id: checkJavaExecutable
                    text: qsTr("Executable:")

                    checked: !!currentProfile.javaDir
                }
                TextField {
                    id: javaExecutable

                    Layout.fillWidth: true
                    Layout.preferredHeight: 21

                    text: currentProfile.javaDir || Launcher.java_executable

                    enabled: checkJavaExecutable.checked
                }

                CheckBox{
                    id: checkJavaArguments
                    text: qsTr("JVM Arguments:")

                    checked: !!currentProfile.javaArgs
                }
                TextField {
                    id: javaArguments
                    Layout.fillWidth: true
                    Layout.preferredHeight: 21

                    text: {
                        var args = currentProfile.javaArgs || Profiles.default_java_args;
                        return qsTr(args);
                    }

                    enabled: checkJavaArguments.checked
                }

            }
        }

        // Buttons
        RowLayout {
            Button {
                text: "Cancel"

                onClicked: control.close()
            }
            Button {
                text: "Open Game Dir"

                onClicked: Qt.openUrlExternally(Launcher.game_directory)
            }
            Button {
                text: "Save Profile"

                onClicked: {
                    Profiles.save(constructProfileJson());
                    control.close();
                }
            }
        }
    }

    function constructProfileJson() {
        let json = {
            "name": profileName.text,
            "lastVersionId": comboUseVersion.currentValue
        };

        checkGameDir.checked ? json["gameDir"] = gameDir.text : json["gameDir"] = "";
        checkJavaExecutable.checked ? json["javaDir"] = javaExecutable.text : json["javaDir"] = "";
        checkJavaArguments.checked ? json["javaArgs"] = javaArguments.text : json["javaArgs"] = "";

        return json;
    }
}