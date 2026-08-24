import QtQuick
import QtQuick.Controls

import jayrickaby.lj_launcher

Action {
    id: root

    property var profileId

    text: qsTr("Delete Profile")

    onTriggered: {
        if (!profileId) return;

        Profiles.deleteProfile(profileId);
    }
}