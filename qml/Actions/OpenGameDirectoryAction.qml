import QtQuick
import QtQuick.Controls

import jayrickaby.lj_launcher

Action {
    id: root

    property var profileId

    text: qsTr("Open Game Folder")

    onTriggered: {
        if (!profileId) return;

        let profile = Profiles.getProfile(profileId)
        let path = profile.gameDir ? profile.gameDir : Launcher.gameDirectory
        let url = path.startsWith("file://") ? path : "file://" + path

        Qt.openUrlExternally(url)
    }
}