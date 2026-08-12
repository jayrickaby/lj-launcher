import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtWebEngine

import jayrickaby.lj_launcher

import "./template"

LauncherPage {
    id: control

    property int authState: Authentication.authState

    Connections {
        target: Launcher

        function onLauncherError(message) {
            setErrorMessage(message.errorFriendly, message.errorTechnical)
        }
    }

    Rectangle {
        anchors.centerIn: parent

        // TODO: Expands from contents on original... is this the min size?
        width: 315

        height: content.implicitHeight + content.anchors.topMargin + content.anchors.bottomMargin

        ColumnLayout {
            id: content

            anchors.fill: parent

            anchors.leftMargin: 16
            anchors.topMargin: 8
            anchors.rightMargin: 16
            anchors.bottomMargin: 8

            spacing: 16

            Image {
                Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                source: "qrc:/jayrickaby/lj_launcher/assets/minecraft_logo.png"
            }

            ColumnLayout {
                Layout.fillWidth: true

                id: errorMessage
                visible: false

                Text {
                    Layout.fillWidth: true
                    id: errorMessageFriendly

                    font.bold: true

                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                }

                Text {
                    Layout.fillWidth: true
                    id: errorMessageNerd

                    font.bold: true
                    font.italic: true
                    font.pointSize: 7

                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                }
            }

            Button {
                id: loginButton
                Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
                Layout.preferredHeight: 21
                Layout.fillWidth: true

                text: authState !== Authentication.AuthState.UNAUTHENTICATED ? "Loading" : "Log in via Microsoft"
                enabled: authState === Authentication.AuthState.UNAUTHENTICATED

                onClicked: {
                    clearErrorMessage();
                    authWindow.visible = true;
                    authWindow.setUrl(Authentication.codeUrl);
                }
            }
        }
    }

    Window {
        id: authWindow
        width: 900
        height: 700
        visible: false

        title: loginButton.text

        modality: Qt.WindowModal

        function setUrl(newUrl) {
            authWeb.url = newUrl;
        }

        WebEngineView {
            id: authWeb
            anchors.fill: parent

            onLoadingChanged: (loadingInfo) => {
                if (Authentication.isUrlLocalhost(url)) return;

                if (loadingInfo.status === WebEngineView.LoadFailedStatus) {
                    authWindow.visible = false;
                    setErrorMessage("UnknownHostException", "login.microsoftonline.com");
                }
            }

            onUrlChanged: {
                handleNewUrl(url);
            }
        }
    }

    function clearErrorMessage() {
        errorMessage.visible = false;
        errorMessageFriendly.text = qsTr("");
        errorMessageNerd.text = qsTr("");
    }

    function handleNewUrl(url) {
        if (!Authentication.isUrlLocalhost(url)) return;

        Authentication.parseLocalhost(url);

        authWindow.visible = false;

        loginButton.enabled = false;
        loginButton.text = qsTr("Loading");
    }

    function setErrorMessage(friendlyError, nerdError) {
        errorMessage.visible = true;

        errorMessageFriendly.text = qsTr(friendlyError);
        errorMessageNerd.text = qsTr(`( ${nerdError} )`);
    }

    background: Image {
        sourceSize.width: 64
        sourceSize.height: 64

        fillMode: Image.Tile

        // fillMode: Image.TileVertically
        source: "qrc:/jayrickaby/lj_launcher/assets/dirt.png"
    }
}