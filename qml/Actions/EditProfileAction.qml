import QtQuick
import QtQuick.Controls

import jayrickaby.lj_launcher

Action {
    id: root

    property var editor
    property var profileId

    text: qsTr("Edit Profile")

    onTriggered: {
        if (!editor) return;
        if (!profileId) return;

        editor.show();
        let profile = Profiles.getProfile(profileId);
        editor.setProfile(profile, profileId);
    }
}