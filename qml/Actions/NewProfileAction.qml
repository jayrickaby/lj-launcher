import QtQuick
import QtQuick.Controls

import jayrickaby.lj_launcher

Action {
    id: root

    property Loader editorLoader: null
    property var profileId: null

    text: qsTr("New Profile")

    onTriggered: {
        if (!editorLoader) return;
        if (!profileId) return;

        editorLoader.active = true;
        let profile = Profiles.getProfileFromId(profileId);
        profile["name"] = `Copy of ${profile["name"]}`;
        editorLoader.item.setProfile(profile, null);
    }
}