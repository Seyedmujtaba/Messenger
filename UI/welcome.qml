import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt5Compat.GraphicalEffects

ApplicationWindow {
    visible: true
    // width: 1200
    // height: 800
    visibility: Window.FullScreen
    title: "Page_Welcome"
    flags: Qt.FramelessWindowHint

    Grid {
        anchors.centerIn: parent
        columns: 2
        rows: 1
        spacing: 50

        Rectangle {
            width: 1000
            height: 500
            color: "#1E2A44"
            radius: 12
            layer.enabled: true
            layer.effect: DropShadow {
                transparentBorder: true
                color: Qt.rgba(0, 0, 0, 0.1)
                radius: 4
                samples: 8
            }
            Grid {
                columns: 2
                rows: 1
                anchors.centerIn: parent

                Item {
                    width: 600
                    height: 500

                    Rectangle {
                        id: mask
                        width: 600
                        height: 500
                        radius: 12
                        color: Qt.rgba(1, 1, 1, 0.8)
                        layer.effect: DropShadow {
                            transparentBorder: true
                            color: Qt.rgba(0, 0, 0, 0.2)
                            radius: 8
                            samples: 16
                        }
                    }

                    Image {
                        id: image
                        source: "qrc:/image.jpg"
                        width: 600
                        height: 500
                        smooth: true
                        visible: false
                    }

                    OpacityMask {
                        anchors.fill: image
                        source: image
                        maskSource: mask
                        z: 1
                    }
                }

                Grid {
                    width: 400
                    height: 500
                    columns: 1
                    spacing: 20
                    verticalItemAlignment: Grid.AlignVCenter
                    horizontalItemAlignment: Grid.AlignHCenter

                    Text {
                        id: title
                        text: "WELCOME TO APP MESSENGER"
                        font.bold: true
                        font.pixelSize: 25
                        color: "#E4E7EB"
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        anchors.topMargin: 150
                    }

                    Text {
                        id: caption
                        width: 350
                        text: "I would like to sincerely appreciate your tireless efforts on this project. Each of you played a key role in its success, and your teamwork and commitment have resulted in the brilliant result we have today."
                        wrapMode: Text.Wrap
                        color: "#B0B0B0"
                        font.pixelSize: 15
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: title.bottom
                        anchors.topMargin: 20
                    }

                    Button {

                        text: "Start"
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: caption.bottom
                        anchors.topMargin: 20
                        background: Rectangle {
                            implicitWidth: 150
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
    }
}
