import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {/*
    width: 1200
    height: 800*/
    visibility: Window.FullScreen
    visible: true
    title: qsTr("LOGIN")
    color: "#1E2A44"
    flags: Qt.FramelessWindowHint

    Rectangle {
        anchors.centerIn: parent
        width: 300
        height: 450
        color: "#80ffffff"
        border.color: "#40ffffff"
        radius: 16

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 20
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Text {
                text: qsTr("Sign up")
                font.pointSize: 25
                font.bold: true
                Layout.alignment: Qt.AlignHCenter
            }

            // ---- فیلدها ----
            ColumnLayout {
                spacing: 15
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true

                TextField {
                    id: email_Input
                    placeholderText: "Enter your email"
                    onAccepted: username.focus = true
                    color: "white"
                    font.pixelSize: 16
                    Layout.preferredWidth: 200
                    background: Rectangle {
                        width: parent.width
                        border.color: "white"
                        border.width: 0
                        anchors.bottom: parent.bottom
                        height: 1
                    }
                }

                TextField {
                    id: username
                    placeholderText: "Enter your username"
                    onAccepted: password_Input.focus = true
                    color: "white"
                    font.pixelSize: 16
                    Layout.preferredWidth: 200
                    background: Rectangle {
                        width: parent.width
                        border.color: "white"
                        border.width: 0
                        anchors.bottom: parent.bottom
                        height: 1
                    }
                }

                TextField {
                    id: password_Input
                    placeholderText: "Enter your password"
                    onAccepted: but_1.focus = true
                    echoMode: TextInput.Password
                    color: "white"
                    font.pixelSize: 16
                    Layout.preferredWidth: 200
                    background: Rectangle {
                        width: parent.width
                        border.color: "white"
                        border.width: 0
                        anchors.bottom: parent.bottom
                        height: 1
                    }
                }
            }

            // ---- Remember me ----
            Row {
                spacing: 5
                Layout.alignment: Qt.AlignHCenter

                RadioButton { id: radioOption }
                Text {
                    color: "white"
                    text: "Remember me"
                    font.pixelSize: 10
                }
            }

            Button {
                id: but_1
                text: "Sign up"
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: 200
                Layout.preferredHeight: 40

                background: Rectangle {
                    implicitWidth: 200
                    implicitHeight: 40
                    color: parent.pressed ? "#45A049" : "#4CAF50"
                    radius: 5
                }
                contentItem: Text {
                    text: parent.text
                    font.pixelSize: 14
                    color: "black"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: {
                    mystackView.push("login.qml")
                }
            }
        }
    }
}
