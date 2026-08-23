import QtQuick
import QtQuick.Controls

import jayrickaby.lj_launcher

Action {
    id: root

    property var profileId

    text: qsTr("Copy Profile")

    onTriggered: {
        if (!profileId) return;

        let profile = Profiles.getProfile(profileId);
        profile["name"] = `Copy of ${profile["name"]}`;
        Profiles.createProfile(profile);
    }
}