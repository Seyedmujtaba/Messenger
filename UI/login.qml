import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
Window {
    // width: 1200
    // height: 800
    visible: true
    title: qsTr("LOGIN")
    color: "#1E2A44"
    flags: Qt.FramelessWindowHint
    visibility: Window.FullScreen
    Rectangle {/*
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter*/
            anchors.centerIn: parent
            width: 300
            height: 450
            color: "#80ffffff"
            border.color: "#40ffffff"
            radius: 16
        ColumnLayout{
            // anchors.top: parent.top
            // anchors.right: parent.right
            // anchors.left: parent.left
            anchors.margins: 25
            anchors.centerIn: parent
            spacing: 10
            // layout.f
            height: 300
            GridLayout{
                columns: 1
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: text1
                    text: qsTr("Login")
                    font.pointSize: 25
                    font.bold: true
                }
            }
            GridLayout{
                    width: 200
                    height: 100
                    columns: 1
                    rows: 2
                    rowSpacing: 20
                    TextField {
                        anchors.fill: parent.fillWidth
                                id: email_Input
                                placeholderText: "Enter your email"
                                onAccepted: password_Input.focus = true
                                color: "white"
                                font.pixelSize: 16
                                background: Rectangle {
                                width: 200
                                border.color: "white"
                                border.width: 0
                                anchors.left: parent.left
                                anchors.right: paren.right
                                anchors.bottom: parent.bottom
                                height: 1


                            }
                        }
                    TextField {
                        anchors.fill: parent.fillWidth
                                id: password_Input
                                placeholderText: "Enter your password"
                                onAccepted: but_1.focus = true
                                echoMode: TextInput.Password
                                color: "white"
                                font.pixelSize: 16
                                background: Rectangle {
                                width: 200
                                border.color: "white"
                                border.width: 0
                                anchors.left: parent.left
                                anchors.right: paren.right
                                anchors.bottom: parent.bottom
                                height: 1


                            }

                        }
                }
            GridLayout{
                columns: 2
                rows: 1
                columnSpacing: 20
                Row {
                    spacing: 1
                    RadioButton {
                        id: radioOption
                        checked: false
                    }
                    Text {
                        color: "white"
                        text: "Remmember me"
                        font.pixelSize: 10
                    }
                }
                Text {
                    id: forgetText
                    color: "white"
                    font.pixelSize: 10
                    text: "Forget password?"

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            mystackView.push("signup.qml")
                        }
                    }
                }



            }

            Grid{
                    rows:1
                anchors.horizontalCenter: parent.horizontalCenter
                    Button {
                        text: "Login"
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: caption.bottom
                        anchors.topMargin: 20
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
                            mystackView.push("chat.qml")
                        }
                    }
                }
            }
        }
    }
