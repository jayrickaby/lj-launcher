import QtQuick
import QtQuick.Controls

import jayrickaby.lj_launcher

Action {
    id: root

    property var profileId

    text: qsTr("Copy Profile")

    onTriggered: {
        if (!profileId) return;

        Profiles.copyProfile(profileId);
    }
}