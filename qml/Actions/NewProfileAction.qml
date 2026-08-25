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

        // reset all parameters
        editor.active = false;
        editor.active = true;

        let profile = Profiles.getProfile(profileId).toMap();
        profile["name"] = `Copy of ${profile["name"]}`;
        editor.item.setProfile(profile, null);

        editor.item.show();
    }
}