import QtQuick
import QtQuick.Controls

import jayrickaby.lj_launcher

Action {
    id: root

    property var profileId: null

    text: qsTr("Delete Profile")

    onTriggered: {
        Profiles.deleteProfileById(profileId);
    }
}   