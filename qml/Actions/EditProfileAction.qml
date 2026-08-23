import QtQuick
import QtQuick.Controls

import jayrickaby.lj_launcher

Action {
    id: root

    property Loader editorLoader: null
    property var profileId: null

    text: qsTr("Edit Profile")

    onTriggered: {
        if (!editorLoader) return;
        if (!profileId) return;

        editorLoader.active = true;
        let profile = Profiles.getProfileFromId(profileId);
        editorLoader.item.setProfile(profile, profileId);
    }
}