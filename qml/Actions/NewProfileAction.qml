import QtQuick
import QtQuick.Controls

import jayrickaby.lj_launcher

Action {
    id: root

    property var editor
    property var profileId

    text: qsTr("New Profile")

    onTriggered: {
        if (!editor) return;
        if (!profileId) return;

        editor.show();
        let profile = Profiles.getProfile(profileId).toMap();
        profile["name"] = `Copy of ${profile["name"]}`;
        editor.setProfile(profile, null);
    }
}