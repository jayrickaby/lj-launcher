import QtQuick
import QtQuick.Controls

import jayrickaby.lj_launcher

Action {
    id: root

    property var editorLoader
    property var profileId

    text: qsTr("Edit Profile")

    onTriggered: {
        if (!editorLoader) return;
        if (!profileId) return;

        editorLoader.active = true;
        let profile = Profiles.getProfile(profileId);
        editorLoader.item.setProfile(profile, profileId);
    }
}