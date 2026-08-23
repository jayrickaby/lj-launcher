import QtQuick
import QtQuick.Controls

Action {
    id: root

    property Loader editorLoader: null
    property var profileId: null

    text: qsTr("Edit Profile")

    onTriggered: {
        if (!editorLoader) return;

        editorLoader.active = true;
        let profile = Profiles.getProfileFromId(profileId);
        editorLoader.item.setProfile(profile, profileId);
    }
}