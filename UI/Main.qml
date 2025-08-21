import QtQuick
import QtQuick.Controls
ApplicationWindow {
    flags: Qt.FramelessWindowHint

    StackView {
        id: mystackView
        anchors.fill: parent
        initialItem: "welcome.qml"
    }
}
