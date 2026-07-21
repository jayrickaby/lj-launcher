import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtWebEngine

import jayrickaby.lj_launcher.authentication 1.0
import jayrickaby.lj_launcher.application 1.0

import "./template"

LauncherPage {
    id: control

    Connections {
        target: Authentication

        function onAuthenticated() {
            loginButton.text = qsTr("Log in via Microsoft");
            loginButton.enabled = true;
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
                source: Qt.resolvedUrl(Application.parent_path + "/qml/assets/minecraft_logo.png")
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

                text: "Log in via Microsoft"

                onClicked: {
                    clearErrorMessage();
                    authWindow.visible = true;
                    authWindow.setUrl(Authentication.login_url);
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

        function setUrl(newUrl) {
            authWeb.url = newUrl;
        }

        WebEngineView {
            id: authWeb
            anchors.fill: parent

            onLoadingChanged: (loadingInfo) => {
                if (Authentication.is_url_localhost(url)) return;

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
        if (!Authentication.is_url_localhost(url)) return;

        let params = Authentication.parse_localhost_url(url);

        authWindow.visible = false;

        if (params.error !== undefined) {
            setErrorMessage(
                params.error,
                params.error_description
            );
            return;
        }

        if (params.code === undefined) {
            return;
        }

        loginButton.enabled = false;
        loginButton.text = qsTr("Loading");

        Authentication.complete_auth(url);
    }

    function setErrorMessage(nerdError, nerdDescription) {
        errorMessage.visible = true;

        let message = "";

        switch (nerdError) {
            // https://datatracker.ietf.org/doc/html/rfc6749
            case "access_denied":
                message = "Sorry, but authentication was denied!\nPlease try again.";
                break;
            case "server_error":
            case "temporarily_unavailable":
                message = "Sorry, but something broke on Microsoft's end!\nPlease try again.";
                break;

            // Custom
            case "UnknownHostException":
                message = "Sorry, but we couldn't connect to the servers.\nPlease make sure that you are online and
                 that Minecraft is not blocked.";
                break;

            default:
                message = "Sorry, but something with the backend went wrong!\nPlease contact Jay.";
                break;
        }

        errorMessageFriendly.text = qsTr(message);
        errorMessageNerd.text = qsTr(`( ${nerdError}: ${nerdDescription} )`);
    }

    background: Image {
        sourceSize.width: 64
        sourceSize.height: 64

        fillMode: Image.Tile

        // fillMode: Image.TileVertically
        source: Qt.resolvedUrl(Application.parent_path + "/qml/assets/dirt.png")
    }
}