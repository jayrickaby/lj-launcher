import QtQuick
import QtQuick.Controls

import jayrickaby.lj_launcher

Action {
    id: root

    property var profileId: null

    text: qsTr("Copy Profile")

    onTriggered: {
        let profile = Profiles.getProfileFromId(profileId);
        Profiles.addNewProfile(profile);
    }
}