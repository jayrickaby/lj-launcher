import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

MessageDialog {
    id: root

    property bool hasAcknowledged: false
    property Item checkBox: null

    informativeText: "Are you sure you want to continue?"
    title: "Select an Option"

    buttons: MessageDialog.Yes | MessageDialog.No | MessageDialog.Cancel

    onButtonClicked: function (button, role) {
        switch (button) {
            case MessageDialog.Yes:
                root.hasAcknowledged = true;
                checkBox.checked = true;
                root.accepted();
                break;
            case MessageDialog.No:
                root.hasAcknowledged = true;
                checkBox.checked = false;
                root.rejected();
                break;
            case MessageDialog.Cancel:
                root.checkBox.checked = false;
                root.rejected();
                root.break;
        }
    }
}