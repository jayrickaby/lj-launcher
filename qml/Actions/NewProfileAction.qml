import QtQuick
import QtQuick.Controls

Action {
    id: root

    property Loader editorLoader: null
    property var profileId: null

    text: qsTr("New Profile")

    onTriggered: {
        if (!editorLoader) return;

        editorLoader.active = true;
        let profile = Profiles.getProfileFromId(profileId);
        profile["name"] = `Copy of ${profile["name"]}`;
        editorLoader.item.setProfile(profile, null);
    }
}