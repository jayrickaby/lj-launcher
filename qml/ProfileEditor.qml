import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import jayrickaby.lj_launcher
import "./ProfileEditor"

ApplicationWindow {
    id: control

    modality: Qt.ApplicationModal

    title: qsTr("Profile Editor")

    width: content.implicitWidth + leftPadding + rightPadding
    height: content.implicitHeight + topPadding + bottomPadding

    minimumWidth: content.implicitWidth + leftPadding + rightPadding
    minimumHeight: content.implicitHeight + topPadding + bottomPadding

    property var currentProfile: null
    property var currentProfileId: null

    property bool showSnapshots: checkShowSnapshotVersions.checked
    property bool showAlphas: checkShowAlphaVersions.checked
    property bool showBetas: checkShowBetaVersions.checked
    
    leftPadding: 8; topPadding: 8; rightPadding: 8; bottomPadding: 8;

    CheckBoxPopup {
        id: alphasPopup

        hasAcknowledged: currentProfile.showAlphaVersions
        text: "These versions are very out of date and may be unstable. Any bugs, crashes, missing features or\nother nasties you may find will never be fixed in these versions.\nIt is strongly recommended you play these in separate directories to avoid corruption.\nWe are not responsible for the damage to your nostalgia or your save files!"
        checkBox: checkShowAlphaVersions
    }
    CheckBoxPopup {
        id: betasPopup

        hasAcknowledged: currentProfile.showBetaVersions
        text: "These versions are very out of date and may be unstable. Any bugs, crashes, missing features or\nother nasties you may find will never be fixed in these versions.\nIt is strongly recommended you play these in separate directories to avoid corruption.\nWe are not responsible for the damage to your nostalgia or your save files!"
        checkBox: checkShowBetaVersions
    }
    CheckBoxPopup {
        id: snapshotsPopup

        hasAcknowledged: currentProfile.showSnapshotVersions
        text: "Are you sure you want to enable development builds?\nThey are not guaranteed to be stable and may corrupt your world.\nYou are advised to run this in a separate directory or run regular backups."
        checkBox: checkShowSnapshotVersions
    }

    ColumnLayout {
        id: content
        anchors.fill: parent
        spacing: 8

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
                        var dir = currentProfile.gameDir || Launcher.gameDirectory;
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
                        id: resolutionWidth

                        Layout.fillWidth: true
                        Layout.preferredHeight: 21

                        inputMethodHints: Qt.ImhDigitsOnly
                        validator: IntValidator {
                            bottom: 0;
                            top: 65535;
                        }

                        text: {
                            var res = currentProfile.resolution || Profiles.defaultResolution;
                            return res.width;
                        }

                        enabled: checkResolution.checked
                    }
                    Text {
                        text: "x"

                        font.pointSize: 8
                    }
                    TextField {
                        id: resolutionHeight

                        Layout.fillWidth: true
                        Layout.preferredHeight: 21

                        inputMethodHints: Qt.ImhDigitsOnly
                        validator: IntValidator {
                            bottom: 0;
                            top: 65535;
                        }

                        text: {
                            var res = currentProfile.resolution || Profiles.defaultResolution;
                            return res.height;
                        }

                        enabled: checkResolution.checked
                    }
                }

                // Row 4
                // CheckBox {
                //     text: qsTr("Automatically ask Mojang for assistance with fixing crashes")
                //
                //     // TODO: Add actual crash stuff
                //     checked: Launcher.settings.crashAssistance
                //
                //     font.pointSize: 8
                //     Layout.columnSpan: 2
                //
                //     onCheckedChanged: Launcher.set_setting("crashAssistance", checked)
                // }

                // Row 5
                // CheckBox {
                //     id: checkLauncherVisible
                //     text: qsTr("Keep Launcher Open")
                //
                //     checked: Launcher.settings.keepLauncherOpen
                //
                //     font.pointSize: 8
                //
                //     onCheckedChanged: Launcher.set_setting("keepLauncherOpen", checked)
                // }
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
                    id: checkShowSnapshotVersions
                    Layout.columnSpan: 2
                    text: qsTr("Enable experimental development versions (\"snapshots\")")

                    checked: currentProfile.showSnapshotVersions
                    onCheckedChanged: {
                        if (checked && !snapshotsPopup.hasAcknowledged) {
                            snapshotsPopup.open();
                        }
                    }
                }
                CheckBox{
                    id: checkShowBetaVersions
                    Layout.columnSpan: 2
                    text: qsTr("Allow use of old \"Beta\" Minecraft versions (From 2010-2011)")

                    checked: currentProfile.showBetaVersions
                    onCheckedChanged: {
                        if (checked && !betasPopup.hasAcknowledged) {
                            betasPopup.open();
                        }
                    }
                }
                CheckBox{
                    id: checkShowAlphaVersions
                    Layout.columnSpan: 2
                    text: qsTr("Allow use of old \"Alpha\" Minecraft versions (From 2010)")

                    checked: currentProfile.showAlphaVersions
                    onCheckedChanged: {
                        if (checked && !alphasPopup.hasAcknowledged) {
                            alphasPopup.open();
                        }
                    }
                }

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

                    model: control.visible
                        ? Versions.getVersionsList(showSnapshots, showBetas, showAlphas)
                        : []
                    currentValue: currentProfile.lastVersionId

                    onActivated: (index) => {
                        let selectedItem = model[index]

                        if (selectedItem) {
                            console.log("Current version changed to:", selectedItem.id);
                        }
                    }
                    onCurrentIndexChanged: {
                        if (control.visible && currentIndex === -1) {
                            currentIndex = 0;
                        }
                    }
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

                    text: currentProfile.javaDir || Launcher.javaExecutable

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
                        var args = currentProfile.javaArgs || Profiles.defaultJavaArgs;
                        return qsTr(args);
                    }

                    enabled: checkJavaArguments.checked
                }

            }
        }

        // Buttons
        RowLayout {
            Layout.fillWidth: true

            Button {
                text: qsTr("Cancel")

                onClicked: control.close()
            }

            // TODO: Better alternative than a spacer item?
            Item {
                Layout.fillWidth: true
            }

            Button {
                text: qsTr("Open Game Dir")

                action:OpenGameDirectoryAction {
                    id: openGameDirAction
                    profileId: currentProfileId
                }
            }
            Button {
                text: qsTr("Save Profile")

                onClicked: {
                    let profile = constructProfileJson();
                    if (currentProfileId == null) {
                        Profiles.createProfile(profile);
                    }
                    else {
                        Profiles.editProfile(currentProfileId, profile);
                    }
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

        if (checkGameDir.checked) {
            json["gameDir"] = gameDir.text;
        }
        else {
            json["gameDir"] = null;
        }
        if (checkJavaExecutable.checked) {
            json["javaDir"] = javaExecutable.text
        }
        else {
            json["javaDir"] = null;
        }
        if (checkJavaArguments.checked) {
            json["javaArgs"] = javaArguments.text
        }
        else {
            json["javaArgs"] = null
        }
        if (checkResolution.checked) {
            json["resolution"] = {
                "width": Number(resolutionWidth.text),
                "height": Number(resolutionHeight.text)
            }
        }
        else {
            json["resolution"] = null
        }

        json["showAlphaVersions"] = showAlphas;
        json["showBetaVersions"] = showBetas;
        json["showSnapshotVersions"] = showSnapshots;

        return json;
    }

    function setProfile(profile, id) {
        if (id == null) {
            console.log("Assuming creating a new profile.");
        }
        else {
            console.log("Assume editing an existing profile.");
        }
        currentProfileId = id;

        if (profile == null) {
            throw new Error("Profile data cannot be empty");
        }
        currentProfile = profile;
    }


}