import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt5Compat.GraphicalEffects

ApplicationWindow {
    id: mainWindow
    visible: true
    // width: 1200
    // height: 800
    title: "َApp messenger"
    flags: Qt.FramelessWindowHint
    visibility: Window.FullScreen

    // Color scheme - redesigned to match the sign-up form's gray tones with green accents
    property color primaryBg: "#1F2937" // Dark gray background
    property color secondaryBg: "#374151" // Medium dark gray
    property color cardBg: "#4B5563" // Lighter gray for cards
    property color accentBlue: "#22C55E" // Green accent for buttons and highlights
    property color accentGold: "#EAB308" // Yellow for secondary accents (e.g., unread counts)
    property color textPrimary: "#F9FAFB" // Light text for readability on dark backgrounds
    property color textSecondary: "#D1D5DB" // Muted light text
    property color textMuted: "#9CA3AF" // Even more muted text
    property color successGreen: "#22C55E" // Green for success/online indicators
    property color borderColor: "#4B5563" // Gray borders
    property color hoverColor: "#374151" // Hover gray

    // Standard sizes
    property int headerHeight: 70
    property int sidebarWidth: 300
    property int inputHeight: 60
    property int avatarSize: 40
    property int smallAvatarSize: 32
    property int buttonSize: 36
    property int borderRadius: 12
    property int smallRadius: 8

    property string currentChatUser: "Team Chat"
    property string currentChatAvatar: "💼"

    ListModel {
        id: contactsModel
        ListElement {
            name: "Sarah Chen"; avatar: "👩"; isOnline: true;
            lastMessage: "Hey everyone! How's your day going?";
            lastTime: "2 min"; unreadCount: 2; isActive: false
        }
        ListElement {
            name: "Mike Johnson"; avatar: "👨"; isOnline: true;
            lastMessage: "Great! Just finished the morning meeting.";
            lastTime: "5 min"; unreadCount: 0; isActive: false
        }
        ListElement {
            name: "Emma Davis"; avatar: "👩"; isOnline: false;
            lastMessage: "Thanks for the update!";
            lastTime: "1h"; unreadCount: 1; isActive: false
        }
        ListElement {
            name: "Alex Wilson"; avatar: "👨"; isOnline: true;
            lastMessage: "Working on the designs now";
            lastTime: "3h"; unreadCount: 0; isActive: false
        }
        ListElement {
            name: "Team Chat"; avatar: "💼"; isOnline: true;
            lastMessage: "You: Working on the new designs...";
            lastTime: "now"; unreadCount: 0; isActive: true
        }
        ListElement {
            name: "Lisa Zhang"; avatar: "👩"; isOnline: true;
            lastMessage: "Let's schedule a meeting";
            lastTime: "2d"; unreadCount: 3; isActive: false
        }
        ListElement {
            name: "Tom Brown"; avatar: "👨"; isOnline: false;
            lastMessage: "Perfect! See you tomorrow";
            lastTime: "1w"; unreadCount: 0; isActive: false
        }
        ListElement {
            name: "Design Team"; avatar: "🎨"; isOnline: true;
            lastMessage: "Kate: New mockups are ready";
            lastTime: "3d"; unreadCount: 5; isActive: false
        }
    }

    // Messages for different chats
    property var chatMessages: ({
        "Team Chat": [
            { sender: "Sarah Chen", text: "Hey everyone! How's your day going?", time: "10:30", avatar: "👩", isMe: false, isImage: false, isVoice: false },
            { sender: "Mike Johnson", text: "Great! Just finished the morning meeting.", time: "10:32", avatar: "👨", isMe: false, isImage: false, isVoice: false },
            { sender: "Me", text: "Working on the new designs. Almost done!", time: "10:35", avatar: "👤", isMe: true, isImage: false, isVoice: false }
        ],
        "Sarah Chen": [
            { sender: "Sarah Chen", text: "Hi! How are you doing?", time: "09:15", avatar: "👩", isMe: false, isImage: false, isVoice: false },
            { sender: "Me", text: "I'm good, thanks! Working on some projects.", time: "09:18", avatar: "👤", isMe: true, isImage: false, isVoice: false }
        ],
        "Mike Johnson": [
            { sender: "Mike Johnson", text: "Can we discuss the project timeline?", time: "11:20", avatar: "👨", isMe: false, isImage: false, isVoice: false },
            { sender: "Me", text: "Sure! When would be good for you?", time: "11:25", avatar: "👤", isMe: true, isImage: false, isVoice: false }
        ],
        "Emma Davis": [
            { sender: "Emma Davis", text: "Thanks for the presentation today!", time: "14:30", avatar: "👩", isMe: false, isImage: false, isVoice: false }
        ],
        "Alex Wilson": [
            { sender: "Alex Wilson", text: "The new designs look amazing!", time: "08:45", avatar: "👨", isMe: false, isImage: false, isVoice: false },
            { sender: "Me", text: "Thank you! I'm still working on some details.", time: "08:50", avatar: "👤", isMe: true, isImage: false, isVoice: false }
        ],
        "Lisa Zhang": [
            { sender: "Lisa Zhang", text: "Let's schedule a meeting for next week", time: "16:20", avatar: "👩", isMe: false, isImage: false, isVoice: false },
            { sender: "Lisa Zhang", text: "I have some new ideas to share", time: "16:21", avatar: "👩", isMe: false, isImage: false, isVoice: false },
            { sender: "Lisa Zhang", text: "What do you think?", time: "16:22", avatar: "👩", isMe: false, isImage: false, isVoice: false }
        ],
        "Tom Brown": [
            { sender: "Tom Brown", text: "Perfect! See you tomorrow", time: "17:45", avatar: "👨", isMe: false, isImage: false, isVoice: false }
        ],
        "Design Team": [
            { sender: "Kate Miller", text: "New mockups are ready for review", time: "13:15", avatar: "👩", isMe: false, isImage: false, isVoice: false },
            { sender: "James Wilson", text: "Great work everyone!", time: "13:20", avatar: "👨", isMe: false, isImage: false, isVoice: false },
            { sender: "Kate Miller", text: "Thanks! Let me know your feedback", time: "13:22", avatar: "👩", isMe: false, isImage: false, isVoice: false },
            { sender: "Me", text: "Looks fantastic! Just a few minor tweaks needed.", time: "13:25", avatar: "👤", isMe: true, isImage: false, isVoice: false },
            { sender: "James Wilson", text: "I agree, overall very solid work", time: "13:30", avatar: "👨", isMe: false, isImage: false, isVoice: false }
        ]
    })

    ListModel {
        id: messagesModel
        ListElement {
            sender: "Sarah Chen"; text: "Hey everyone! How's your day going?";
            time: "10:30"; avatar: "👩"; isMe: false; isImage: false; isVoice: false
        }
        ListElement {
            sender: "Mike Johnson"; text: "Great! Just finished the morning meeting.";
            time: "10:32"; avatar: "👨"; isMe: false; isImage: false; isVoice: false
        }
        ListElement {
            sender: "Me"; text: "Working on the new designs. Almost done!";
            time: "10:35"; avatar: "👤"; isMe: true; isImage: false; isVoice: false
        }
        ListElement {
            sender: "Alex Wilson"; text: "";
            time: "10:40"; avatar: "👨"; isMe: false; isImage: true; isVoice: false
        }
    }

    // Main background with gradient
    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: primaryBg }
            GradientStop { position: 1.0; color: Qt.darker(primaryBg, 1.1) }
        }

        // Main layout
        RowLayout {
            anchors.fill: parent
            anchors.margins: 16
            spacing: 16

            // Left Sidebar - Contacts List
            Rectangle {
                Layout.preferredWidth: sidebarWidth
                Layout.fillHeight: true
                color: secondaryBg
                radius: borderRadius
                border.color: borderColor
                border.width: 1

                // Subtle glow effect
                layer.enabled: true
                layer.effect: DropShadow {
                    transparentBorder: true
                    color: Qt.rgba(34, 197, 94, 0.15)
                    radius: 8
                    samples: 16
                }

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 0

                    // Header with gradient
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: headerHeight
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: cardBg }
                            GradientStop { position: 1.0; color: Qt.darker(cardBg, 1.05) }
                        }
                        radius: borderRadius

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 20

                            Text {
                                text: "💬 Chats"
                                color: textPrimary
                                font.pixelSize: 22
                                font.weight: Font.Bold
                                font.family: "SF Pro Display"
                            }

                            Item { Layout.fillWidth: true }

                            Rectangle {
                                width: buttonSize
                                height: buttonSize
                                radius: buttonSize / 2
                                gradient: Gradient {
                                    GradientStop { position: 0.0; color: newChatArea.containsMouse ? accentBlue : accentGold }
                                    GradientStop { position: 1.0; color: newChatArea.containsMouse ? Qt.lighter(accentBlue, 1.2) : Qt.lighter(accentGold, 1.1) }
                                }
                                border.color: borderColor
                                border.width: 1

                                Text {
                                    anchors.centerIn: parent
                                    text: "✏️"
                                    font.pixelSize: 16
                                }

                                MouseArea {
                                    id: newChatArea
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: showNotification("New chat")
                                }

                                Behavior on scale {
                                    NumberAnimation { duration: 150 }
                                }

                                scale: newChatArea.pressed ? 0.95 : 1.0
                            }
                        }
                    }

                    // Search bar with gradient
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 60
                        color: "transparent"

                        Rectangle {
                            anchors.fill: parent
                            anchors.margins: 10
                            // anchors.topMargin: 20
                            gradient: Gradient {
                                GradientStop { position: 0.0; color: cardBg }
                                GradientStop { position: 1.0; color: Qt.darker(cardBg, 1.05) }
                            }
                            radius: smallRadius
                            border.color: searchInput.activeFocus ? accentBlue : borderColor
                            border.width: 1

                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 10
                                // spacing: 8

                                Text {
                                    text: "🔍"
                                    color: textMuted
                                    font.pixelSize: 16
                                }

                                TextInput {
                                    id: searchInput
                                    Layout.fillWidth: true
                                    color: textPrimary
                                    font.pixelSize: 14
                                    font.family: "SF Pro Text"
                                    selectByMouse: true

                                    Text {
                                        text: "Search conversations..."
                                        color: textMuted
                                        visible: parent.text.length === 0
                                        font: parent.font
                                    }
                                }
                            }

                            Behavior on border.color {
                                ColorAnimation { duration: 200 }
                            }
                        }
                    }

                    // Contacts list
                    ListView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        model: contactsModel
                        clip: true
                        spacing: 2

                        delegate: Rectangle {
                            width: ListView.view.width
                            height: 72
                            gradient: Gradient {
                                GradientStop { position: 0.0; color: model.isActive ? Qt.rgba(34, 197, 94, 0.2) : (contactArea.containsMouse ? hoverColor : "transparent") }
                                GradientStop { position: 1.0; color: model.isActive ? Qt.rgba(34, 197, 94, 0.1) : (contactArea.containsMouse ? Qt.darker(hoverColor, 1.05) : "transparent") }
                            }
                            radius: smallRadius

                            Rectangle {
                                width: 4
                                height: parent.height - 16
                                anchors.left: parent.left
                                anchors.leftMargin: 8
                                anchors.verticalCenter: parent.verticalCenter
                                gradient: Gradient {
                                    GradientStop { position: 0.0; color: accentBlue }
                                    GradientStop { position: 1.0; color: accentGold }
                                }
                                radius: 2
                                visible: model.isActive
                            }

                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 16
                                spacing: 12

                                // Avatar with status and glow
                                Item {
                                    Layout.preferredWidth: avatarSize
                                    Layout.preferredHeight: avatarSize

                                    Rectangle {
                                        width: avatarSize
                                        height: avatarSize
                                        radius: avatarSize / 2
                                        gradient: Gradient {
                                            GradientStop { position: 0.0; color: cardBg }
                                            GradientStop { position: 1.0; color: Qt.darker(cardBg, 1.1) }
                                        }
                                        border.color: model.isOnline ? successGreen : borderColor
                                        border.width: 2

                                        // Glow effect for online users
                                        layer.enabled: model.isOnline
                                        layer.effect: DropShadow {
                                            transparentBorder: true
                                            color: Qt.rgba(34, 197, 94, 0.3)
                                            radius: 6
                                            samples: 12
                                        }

                                        Text {
                                            anchors.centerIn: parent
                                            text: model.avatar
                                            font.pixelSize: 18
                                        }

                                        Rectangle {
                                            width: 12
                                            height: 12
                                            radius: 6
                                            color: model.isOnline ? successGreen : textMuted
                                            anchors.bottom: parent.bottom
                                            anchors.right: parent.right
                                            anchors.rightMargin: -2
                                            anchors.bottomMargin: -2
                                            border.color: secondaryBg
                                            border.width: 2
                                            visible: model.isOnline || !model.isOnline

                                            // Pulse animation for online status
                                            SequentialAnimation on scale {
                                                running: model.isOnline
                                                loops: Animation.Infinite
                                                NumberAnimation { to: 1.2; duration: 1000 }
                                                NumberAnimation { to: 1.0; duration: 1000 }
                                            }
                                        }
                                    }
                                }

                                // Content
                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 4

                                    RowLayout {
                                        Layout.fillWidth: true

                                        Text {
                                            Layout.fillWidth: true
                                            text: model.name
                                            color: model.isActive ? accentBlue : textPrimary
                                            font.pixelSize: 15
                                            font.weight: Font.DemiBold
                                            font.family: "SF Pro Display"
                                            elide: Text.ElideRight
                                        }

                                        Text {
                                            text: model.lastTime
                                            color: textMuted
                                            font.pixelSize: 12
                                            font.family: "SF Pro Text"
                                        }
                                    }

                                    RowLayout {
                                        Layout.fillWidth: true

                                        Text {
                                            Layout.fillWidth: true
                                            text: model.lastMessage
                                            color: textSecondary
                                            font.pixelSize: 13
                                            font.family: "SF Pro Text"
                                            elide: Text.ElideRight
                                            maximumLineCount: 1
                                        }

                                        Rectangle {
                                            width: 20
                                            height: 20
                                            radius: 10
                                            gradient: Gradient {
                                                GradientStop { position: 0.0; color: accentGold }
                                                GradientStop { position: 1.0; color: Qt.darker(accentGold, 1.1) }
                                            }
                                            visible: model.unreadCount > 0

                                            Text {
                                                anchors.centerIn: parent
                                                text: model.unreadCount > 9 ? "9+" : model.unreadCount.toString()
                                                color: primaryBg
                                                font.pixelSize: 10
                                                font.weight: Font.Bold
                                                font.family: "SF Pro Text"
                                            }

                                            // Pulse animation for unread messages
                                            SequentialAnimation on scale {
                                                running: model.unreadCount > 0
                                                loops: Animation.Infinite
                                                NumberAnimation { to: 1.1; duration: 800 }
                                                NumberAnimation { to: 1.0; duration: 800 }
                                            }
                                        }
                                    }
                                }
                            }

                            MouseArea {
                                id: contactArea
                                anchors.fill: parent
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor
                                onClicked: {
                                    // Reset all active states
                                    for (var i = 0; i < contactsModel.count; i++) {
                                        contactsModel.setProperty(i, "isActive", false)
                                        contactsModel.setProperty(i, "unreadCount", 0)
                                    }
                                    // Set clicked item as active
                                    contactsModel.setProperty(index, "isActive", true)

                                    // Update current chat info
                                    currentChatUser = model.name
                                    currentChatAvatar = model.avatar

                                    // Load messages for this chat
                                    loadMessagesForChat(model.name)
                                }
                            }

                            Behavior on color {
                                ColorAnimation { duration: 200 }
                            }
                        }
                    }
                }
            }

            // Main chat area
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                gradient: Gradient {
                    GradientStop { position: 0.0; color: secondaryBg }
                    GradientStop { position: 1.0; color: Qt.darker(secondaryBg, 1.05) }
                }
                radius: borderRadius
                border.color: borderColor
                border.width: 1

                // Chat area glow
                layer.enabled: true
                layer.effect: DropShadow {
                    transparentBorder: true
                    color: Qt.rgba(34, 197, 94, 0.1)
                    radius: 10
                    samples: 20
                }

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 0

                    // Chat header with gradient
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: headerHeight
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: cardBg }
                            GradientStop { position: 1.0; color: Qt.darker(cardBg, 1.05) }
                        }
                        radius: borderRadius
                        border.color: borderColor
                        border.width: 1

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 20

                            // Left side - chat info
                            RowLayout {
                                spacing: 12

                                Rectangle {
                                    width: avatarSize + 4
                                    height: avatarSize + 4
                                    radius: (avatarSize + 4) / 2
                                    gradient: Gradient {
                                        GradientStop { position: 0.0; color: cardBg }
                                        GradientStop { position: 1.0; color: Qt.darker(cardBg, 1.1) }
                                    }
                                    border.color: successGreen
                                    border.width: 2

                                    // Avatar glow
                                    layer.enabled: true
                                    layer.effect: DropShadow {
                                        transparentBorder: true
                                        color: Qt.rgba(34, 197, 94, 0.3)
                                        radius: 6
                                        samples: 12
                                    }

                                    Text {
                                        anchors.centerIn: parent
                                        text: currentChatAvatar
                                        font.pixelSize: 20
                                    }
                                }

                                Column {
                                    spacing: 2

                                    Text {
                                        text: currentChatUser
                                        color: textPrimary
                                        font.pixelSize: 18
                                        font.weight: Font.DemiBold
                                        font.family: "SF Pro Display"
                                    }

                                    Text {
                                        text: currentChatUser === "Team Chat" || currentChatUser === "Design Team" ? "8 members • Online" : "Online"
                                        color: textSecondary
                                        font.pixelSize: 13
                                        font.family: "SF Pro Display"
                                    }
                                }
                            }

                            Item { Layout.fillWidth: true } // Spacer

                            // Right side - action buttons
                            RowLayout {
                                spacing: 5

                                Repeater {
                                    model: [
                                        {icon: "📞", tooltip: "Voice Call" , color:"black"},
                                        {icon: "🎥", tooltip: "Video Call"},
                                        {icon: "⚙️", tooltip: "Settings"}
                                    ]

                                    Rectangle {
                                        width: buttonSize
                                        height: buttonSize
                                        radius: buttonSize / 2
                                        gradient: Gradient {
                                            GradientStop { position: 0.0; color: buttonArea.containsMouse ? accentBlue : "transparent" }
                                            GradientStop { position: 1.0; color: buttonArea.containsMouse ? Qt.lighter(accentBlue, 1.1) : "transparent" }
                                        }
                                        border.color: borderColor
                                        border.width: 1

                                        Text {
                                            anchors.centerIn: parent
                                            text: modelData.icon
                                            font.pixelSize: 16
                                            color: buttonArea.containsMouse ? textPrimary : textSecondary
                                        }

                                        MouseArea {
                                            id: buttonArea
                                            anchors.fill: parent
                                            hoverEnabled: true
                                            cursorShape: Qt.PointingHandCursor
                                            onClicked: showNotification(modelData.tooltip)
                                        }

                                        Behavior on scale {
                                            NumberAnimation { duration: 150 }
                                        }

                                        scale: buttonArea.pressed ? 0.95 : 1.0
                                    }
                                }
                            }
                        }
                    }

                    // Messages area
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: "transparent"

                        ListView {
                            id: messagesView
                            anchors.fill: parent
                            anchors.margins: 20
                            model: messagesModel
                            spacing: 16
                            clip: true

                            delegate: Item {
                                width: messagesView.width
                                height: messageContent.height + 20

                                RowLayout {
                                    id: messageContent
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    spacing: 12

                                    // Avatar
                                    Rectangle {
                                        width: smallAvatarSize
                                        height: smallAvatarSize
                                        radius: smallAvatarSize / 2
                                        gradient: Gradient {
                                            GradientStop { position: 0.0; color: cardBg }
                                            GradientStop { position: 1.0; color: Qt.darker(cardBg, 1.1) }
                                        }
                                        border.color: borderColor
                                        border.width: 1
                                        visible: !model.isMe

                                        Text {
                                            anchors.centerIn: parent
                                            text: model.avatar
                                            font.pixelSize: 16
                                        }
                                    }

                                    // Message bubble with gradient
                                    Rectangle {
                                        Layout.maximumWidth: messagesView.width * 0.7
                                        Layout.minimumWidth: messageText.implicitWidth + 24
                                        height: messageText.implicitHeight + 16
                                        radius: smallRadius
                                        gradient: Gradient {
                                            GradientStop { position: 0.0; color: model.isMe ? accentBlue : cardBg }
                                            GradientStop { position: 1.0; color: model.isMe ? Qt.darker(accentBlue, 1.1) : Qt.darker(cardBg, 1.05) }
                                        }
                                        border.color: model.isMe ? "transparent" : borderColor
                                        border.width: 1

                                        Layout.alignment: model.isMe ? Qt.AlignRight : Qt.AlignLeft

                                        // Message glow for own messages
                                        layer.enabled: model.isMe
                                        layer.effect: DropShadow {
                                            transparentBorder: true
                                            color: Qt.rgba(34, 197, 94, 0.2)
                                            radius: 4
                                            samples: 8
                                        }

                                        // Image message
                                        Rectangle {
                                            anchors.fill: parent
                                            radius: parent.radius
                                            gradient: Gradient {
                                                GradientStop { position: 0.0; color: "#3A4B6B" }
                                                GradientStop { position: 1.0; color: "#2D3E61" }
                                            }
                                            visible: model.isImage

                                            Text {
                                                anchors.centerIn: parent
                                                text: "🖼️ Image"
                                                color: textSecondary
                                                font.pixelSize: 14
                                            }
                                        }

                                        // Voice message
                                        RowLayout {
                                            anchors.fill: parent
                                            anchors.margins: 12
                                            visible: model.isVoice

                                            Text {
                                                text: "🎵"
                                                font.pixelSize: 16
                                            }

                                            Text {
                                                text: "Voice message"
                                                color: textSecondary
                                                font.pixelSize: 14
                                            }
                                        }

                                        // Text message
                                        Text {
                                            id: messageText
                                            anchors.fill: parent
                                            anchors.margins: 12
                                            text: model.text
                                            color: model.isMe ? textPrimary : textPrimary
                                            font.pixelSize: 14
                                            font.family: "SF Pro Text"
                                            wrapMode: Text.Wrap
                                            verticalAlignment: Text.AlignVCenter
                                            visible: !model.isImage && !model.isVoice && model.text !== ""
                                        }
                                    }

                                    // Spacer for alignment
                                    Item {
                                        Layout.fillWidth: true
                                        visible: !model.isMe
                                    }
                                }

                                // Time and sender
                                Text {
                                    anchors.top: messageContent.bottom
                                    anchors.topMargin: 4
                                    anchors.right: model.isMe ? parent.right : undefined
                                    anchors.left: model.isMe ? undefined : messageContent.left
                                    anchors.leftMargin: model.isMe ? 0 : 44
                                    text: (!model.isMe ? model.sender + " • " : "") + model.time
                                    color: textMuted
                                    font.pixelSize: 11
                                    font.family: "SF Pro Text"
                                }
                            }

                            onCountChanged: positionViewAtEnd()
                        }
                    }

                    // Input area with gradient
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: inputHeight
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: cardBg }
                            GradientStop { position: 1.0; color: Qt.darker(cardBg, 1.05) }
                        }
                        radius: borderRadius
                        border.color: borderColor
                        border.width: 1

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 14
                            spacing: 12

                            // Text input with gradient background
                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 36
                                gradient: Gradient {
                                    GradientStop { position: 0.0; color: secondaryBg }
                                    GradientStop { position: 1.0; color: Qt.darker(secondaryBg, 1.05) }
                                }
                                radius: 10
                                border.color: messageInput.activeFocus ? accentBlue : borderColor
                                border.width: 1

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 8
                                    spacing: 8

                                    TextInput {
                                        id: messageInput
                                        Layout.fillWidth: true
                                        color: textPrimary
                                        font.pixelSize: 14
                                        font.family: "SF Pro Text"
                                        selectByMouse: true

                                        Keys.onReturnPressed: {
                                            sendMessage(text)
                                            text = ""
                                        }

                                        Text {
                                            text: "Type a message..."
                                            color: textMuted
                                            visible: parent.text.length === 0
                                            font: parent.font
                                        }
                                    }

                                    Text {
                                        text: "😊"
                                        font.pixelSize: 16
                                        color: textMuted

                                        MouseArea {
                                            anchors.fill: parent
                                            anchors.margins: -4
                                            cursorShape: Qt.PointingHandCursor
                                            onClicked: emojiPicker.visible = !emojiPicker.visible
                                        }
                                    }
                                }

                                Behavior on border.color {
                                    ColorAnimation { duration: 200 }
                                }
                            }

                            // Action buttons with gradients
                            RowLayout {
                                spacing: 6

                                Repeater {
                                    model: [
                                        {icon: "📎", action: "attach"},
                                        {icon: "🎤", action: "voice"},
                                        {icon: "➤", action: "send"}
                                    ]

                                    Rectangle {
                                        width: buttonSize
                                        height: buttonSize
                                        radius: buttonSize / 2
                                        gradient: Gradient {
                                            GradientStop { position: 0.0;
                                                color: {
                                                    if (modelData.action === "send") {
                                                        return messageInput.text.length > 0 ? accentBlue : cardBg
                                                    }
                                                    return actionArea.containsMouse ? accentBlue : cardBg
                                                }
                                            }
                                            GradientStop { position: 1.0;
                                                color: {
                                                    if (modelData.action === "send") {
                                                        return messageInput.text.length > 0 ? Qt.lighter(accentBlue, 1.1) : Qt.darker(cardBg, 1.05)
                                                    }
                                                    return actionArea.containsMouse ? Qt.lighter(accentBlue, 1.1) : Qt.darker(cardBg, 1.05)
                                                }
                                            }
                                        }
                                        border.color: borderColor
                                        border.width: 1

                                        // Glow effect for send button
                                        layer.enabled: modelData.action === "send" && messageInput.text.length > 0
                                        layer.effect: DropShadow {
                                            transparentBorder: true
                                            color: Qt.rgba(34, 197, 94, 0.3)
                                            radius: 6
                                            samples: 12
                                        }

                                        Text {
                                            anchors.centerIn: parent
                                            text: modelData.icon
                                            font.pixelSize: 16
                                            color: {
                                                if (modelData.action === "send") {
                                                    return messageInput.text.length > 0 ? textPrimary : textMuted
                                                }
                                                return actionArea.containsMouse ? textPrimary : textSecondary
                                            }
                                        }

                                        MouseArea {
                                            id: actionArea
                                            anchors.fill: parent
                                            hoverEnabled: true
                                            cursorShape: Qt.PointingHandCursor
                                            enabled: modelData.action !== "send" || messageInput.text.length > 0
                                            onClicked: {
                                                if (modelData.action === "send") {
                                                    sendMessage(messageInput.text)
                                                    messageInput.text = ""
                                                } else {
                                                    showNotification(modelData.action + " feature coming soon!")
                                                }
                                            }
                                        }

                                        Behavior on scale {
                                            NumberAnimation { duration: 150 }
                                        }

                                        scale: actionArea.pressed ? 0.95 : 1.0
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // Notification toast with gradient
    Rectangle {
        id: notificationToast
        width: 300
        height: 50
        radius: 15
        gradient: Gradient {
            GradientStop { position: 0.0; color: cardBg }
            GradientStop { position: 1.0; color: Qt.darker(cardBg, 1.05) }
        }
        border.color: accentBlue
        border.width: 1
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 80
        anchors.horizontalCenter: parent.horizontalCenter
        opacity: 0
        scale: 0.8

        // Toast glow effect
        layer.enabled: true
        layer.effect: DropShadow {
            transparentBorder: true
            color: Qt.rgba(34, 197, 94, 0.3)
            radius: 8
            samples: 16
        }

        property string message: ""

        Text {
            anchors.centerIn: parent
            text: parent.message
            color: textPrimary
            font.pixelSize: 14
            font.family: "SF Pro Text"
        }

        SequentialAnimation {
            id: showNotificationAnim
            ParallelAnimation {
                NumberAnimation { target: notificationToast; property: "opacity"; to: 1; duration: 200 }
                NumberAnimation { target: notificationToast; property: "scale"; to: 1; duration: 200 }
            }
            PauseAnimation { duration: 2000 }
            ParallelAnimation {
                NumberAnimation { target: notificationToast; property: "opacity"; to: 0; duration: 200 }
                NumberAnimation { target: notificationToast; property: "scale"; to: 0.8; duration: 200 }
            }
        }
    }

    // Timer for auto-replies
    Timer {
        id: responseTimer
        interval: 2000
        onTriggered: {
            var responses = [
                "hello evry body! 👍",
                "That's awesome! 🎉",
                "Thanks for the update!",
                "Great work everyone! 💪",
                "Let's keep it up! 🚀"
            ]

            var randomResponse = responses[Math.floor(Math.random() * responses.length)]
            var randomUser = Math.floor(Math.random() * 3)
            var usernames = ["Sarah Chen", "Mike Johnson", "Alex Wilson"]
            var avatars = ["👩", "👨", "👨"]

            messagesModel.append({
                "sender": usernames[randomUser],
                "text": randomResponse,
                "time": Qt.formatTime(new Date(), "hh:mm"),
                "avatar": avatars[randomUser],
                "isMe": false,
                "isImage": false,
                "isVoice": false
            })

            messagesView.positionViewAtEnd()
        }
    }

    // Functions
    function loadMessagesForChat(chatName) {
        messagesModel.clear()
        var messages = chatMessages[chatName] || []
        for (var i = 0; i < messages.length; i++) {
            messagesModel.append(messages[i])
        }
        messagesView.positionViewAtEnd()
    }

    function sendMessage(text) {
        if (text.trim() === "") return

        var newMessage = {
            "sender": "Me",
            "text": text,
            "time": Qt.formatTime(new Date(), "hh:mm"),
            "avatar": "👤",
            "isMe": true,
            "isImage": false,
            "isVoice": false
        }

        messagesModel.append(newMessage)

        // Add to chat history
        if (!chatMessages[currentChatUser]) {
            chatMessages[currentChatUser] = []
        }
        chatMessages[currentChatUser].push(newMessage)

        messagesView.positionViewAtEnd()
        responseTimer.start()
    }

    function showNotification(message) {
        notificationToast.message = message
        showNotificationAnim.start()
    }

    // Initialize with Team Chat
    Component.onCompleted: {
        loadMessagesForChat("Team Chat")
    }

    // Emoji Picker with redesigned colors
    Rectangle {
        id: emojiPicker
        width: 350
        height: 400
        radius: borderRadius
        gradient: Gradient {
            GradientStop { position: 0.0; color: cardBg }
            GradientStop { position: 1.0; color: Qt.darker(cardBg, 1.05) }
        }
        border.color: borderColor
        border.width: 1
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.margins: 80
        anchors.bottomMargin: 140
        visible: false
        z: 1000

        // Enhanced shadow effect
        layer.enabled: true
        layer.effect: DropShadow {
            transparentBorder: true
            color: Qt.rgba(31, 41, 55, 0.4) // Adjusted to match new primaryBg
            radius: 16
            samples: 32
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 16
            spacing: 12

            // Header with gradient
            RowLayout {
                Layout.fillWidth: true

                Text {
                    text: "✨ Emojis"
                    color: textPrimary
                    font.pixelSize: 16
                    font.weight: Font.Bold
                    font.family: "SF Pro Display"
                }

                Item { Layout.fillWidth: true }

                Rectangle {
                    width: 24
                    height: 24
                    radius: 12
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: closeEmojiArea.containsMouse ? accentBlue : "transparent" }
                        GradientStop { position: 1.0; color: closeEmojiArea.containsMouse ? Qt.lighter(accentBlue, 1.1) : "transparent" }
                    }

                    Text {
                        anchors.centerIn: parent
                        text: "✕"
                        color: textSecondary
                        font.pixelSize: 12
                    }

                    MouseArea {
                        id: closeEmojiArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: emojiPicker.visible = false
                    }
                }
            }

            // Categories with gradients
            RowLayout {
                Layout.fillWidth: true
                spacing: 4

                property int currentCategory: 0

                Repeater {
                    model: [
                        {name: "Smileys", icon: "😀", category: 0},
                        {name: "People", icon: "👤", category: 1},
                        {name: "Nature", icon: "🌿", category: 2},
                    ]

                    Rectangle {
                        width: 40
                        height: 32
                        radius: 6
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: parent.currentCategory === modelData.category ? accentBlue : (categoryArea.containsMouse ? secondaryBg : "transparent") }
                            GradientStop { position: 1.0; color: parent.currentCategory === modelData.category ? Qt.lighter(accentBlue, 1.1) : (categoryArea.containsMouse ? Qt.darker(secondaryBg, 1.05) : "transparent") }
                        }

                        Text {
                            anchors.centerIn: parent
                            text: modelData.icon
                            font.pixelSize: 16
                        }

                        MouseArea {
                            id: categoryArea
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: parent.parent.currentCategory = modelData.category
                        }

                        Behavior on scale {
                            NumberAnimation { duration: 150 }
                        }

                        scale: categoryArea.pressed ? 0.95 : 1.0
                    }
                }
            }

            // Emojis Grid
            GridView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                cellWidth: 40
                cellHeight: 40
                clip: true

                model: ListModel {
                    id: emojiModel

                    // Smileys & Emotion
                    ListElement { emoji: "😀"; category: 0 }
                    ListElement { emoji: "😁"; category: 0 }
                    ListElement { emoji: "😂"; category: 0 }
                    ListElement { emoji: "🤣"; category: 0 }
                    ListElement { emoji: "😃"; category: 0 }
                    ListElement { emoji: "😄"; category: 0 }
                    ListElement { emoji: "😅"; category: 0 }
                    ListElement { emoji: "😆"; category: 0 }
                    ListElement { emoji: "😉"; category: 0 }
                    ListElement { emoji: "😊"; category: 0 }
                    ListElement { emoji: "😋"; category: 0 }
                    ListElement { emoji: "😎"; category: 0 }
                    ListElement { emoji: "😍"; category: 0 }
                    ListElement { emoji: "😘"; category: 0 }
                    ListElement { emoji: "🥰"; category: 0 }
                    ListElement { emoji: "😗"; category: 0 }
                    ListElement { emoji: "😙"; category: 0 }
                    ListElement { emoji: "😚"; category: 0 }
                    ListElement { emoji: "🙂"; category: 0 }
                    ListElement { emoji: "🤗"; category: 0 }
                    ListElement { emoji: "🤩"; category: 0 }
                    ListElement { emoji: "🤔"; category: 0 }
                    ListElement { emoji: "🤨"; category: 0 }
                    ListElement { emoji: "😐"; category: 0 }
                    ListElement { emoji: "😑"; category: 0 }
                    ListElement { emoji: "😶"; category: 0 }
                    ListElement { emoji: "🙄"; category: 0 }
                    ListElement { emoji: "😏"; category: 0 }
                    ListElement { emoji: "😣"; category: 0 }
                    ListElement { emoji: "😥"; category: 0 }
                    ListElement { emoji: "😮"; category: 0 }
                    ListElement { emoji: "🤐"; category: 0 }
                    ListElement { emoji: "😯"; category: 0 }
                    ListElement { emoji: "😪"; category: 0 }
                    ListElement { emoji: "😫"; category: 0 }
                    ListElement { emoji: "🥱"; category: 0 }
                    ListElement { emoji: "😴"; category: 0 }
                    ListElement { emoji: "😌"; category: 0 }
                    ListElement { emoji: "😛"; category: 0 }
                    ListElement { emoji: "😜"; category: 0 }
                    ListElement { emoji: "😝"; category: 0 }
                    ListElement { emoji: "🤤"; category: 0 }
                    ListElement { emoji: "😒"; category: 0 }
                    ListElement { emoji: "😓"; category: 0 }
                    ListElement { emoji: "😔"; category: 0 }
                    ListElement { emoji: "😕"; category: 0 }
                    ListElement { emoji: "🙃"; category: 0 }
                    ListElement { emoji: "🤑"; category: 0 }
                    ListElement { emoji: "😲"; category: 0 }
                    ListElement { emoji: "🙁"; category: 0 }
                    ListElement { emoji: "😖"; category: 0 }
                    ListElement { emoji: "😞"; category: 0 }
                    ListElement { emoji: "😟"; category: 0 }
                    ListElement { emoji: "😤"; category: 0 }
                    ListElement { emoji: "😢"; category: 0 }
                    ListElement { emoji: "😭"; category: 0 }
                    ListElement { emoji: "😦"; category: 0 }
                    ListElement { emoji: "😧"; category: 0 }
                    ListElement { emoji: "😨"; category: 0 }
                    ListElement { emoji: "😩"; category: 0 }
                    ListElement { emoji: "🤯"; category: 0 }
                    ListElement { emoji: "😬"; category: 0 }
                    ListElement { emoji: "😰"; category: 0 }
                    ListElement { emoji: "😱"; category: 0 }
                    ListElement { emoji: "🥵"; category: 0 }
                    ListElement { emoji: "🥶"; category: 0 }
                    ListElement { emoji: "😳"; category: 0 }
                    ListElement { emoji: "🤪"; category: 0 }
                    ListElement { emoji: "😵"; category: 0 }
                    ListElement { emoji: "🥴"; category: 0 }
                    ListElement { emoji: "😷"; category: 0 }
                    ListElement { emoji: "🤒"; category: 0 }
                    ListElement { emoji: "🤕"; category: 0 }
                    ListElement { emoji: "🤢"; category: 0 }
                    ListElement { emoji: "🤮"; category: 0 }
                    ListElement { emoji: "🤧"; category: 0 }
                    ListElement { emoji: "😇"; category: 0 }
                    ListElement { emoji: "🥳"; category: 0 }

                    // People & Body
                    ListElement { emoji: "👋"; category: 1 }
                    ListElement { emoji: "🤚"; category: 1 }
                    ListElement { emoji: "🖐️"; category: 1 }
                    ListElement { emoji: "✋"; category: 1 }
                    ListElement { emoji: "🖖"; category: 1 }
                    ListElement { emoji: "👌"; category: 1 }
                    ListElement { emoji: "🤌"; category: 1 }
                    ListElement { emoji: "🤏"; category: 1 }
                    ListElement { emoji: "✌️"; category: 1 }
                    ListElement { emoji: "🤞"; category: 1 }
                    ListElement { emoji: "🤟"; category: 1 }
                    ListElement { emoji: "🤘"; category: 1 }
                    ListElement { emoji: "🤙"; category: 1 }
                    ListElement { emoji: "👈"; category: 1 }
                    ListElement { emoji: "👉"; category: 1 }
                    ListElement { emoji: "👆"; category: 1 }
                    ListElement { emoji: "🖕"; category: 1 }
                    ListElement { emoji: "👇"; category: 1 }
                    ListElement { emoji: "☝️"; category: 1 }
                    ListElement { emoji: "👍"; category: 1 }
                    ListElement { emoji: "👎"; category: 1 }
                    ListElement { emoji: "✊"; category: 1 }
                    ListElement { emoji: "👊"; category: 1 }
                    ListElement { emoji: "🤛"; category: 1 }
                    ListElement { emoji: "🤜"; category: 1 }
                    ListElement { emoji: "👏"; category: 1 }
                    ListElement { emoji: "🙌"; category: 1 }
                    ListElement { emoji: "👐"; category: 1 }
                    ListElement { emoji: "🤲"; category: 1 }
                    ListElement { emoji: "🤝"; category: 1 }
                    ListElement { emoji: "🙏"; category: 1 }
                    ListElement { emoji: "✍️"; category: 1 }
                    ListElement { emoji: "💅"; category: 1 }
                    ListElement { emoji: "🤳"; category: 1 }
                    ListElement { emoji: "💪"; category: 1 }
                    ListElement { emoji: "🦾"; category: 1 }
                    ListElement { emoji: "🦿"; category: 1 }
                    ListElement { emoji: "🦵"; category: 1 }
                    ListElement { emoji: "🦶"; category: 1 }
                    ListElement { emoji: "👂"; category: 1 }
                    ListElement { emoji: "🦻"; category: 1 }
                    ListElement { emoji: "👃"; category: 1 }
                    ListElement { emoji: "🧠"; category: 1 }
                    ListElement { emoji: "🫀"; category: 1 }
                    ListElement { emoji: "🫁"; category: 1 }
                    ListElement { emoji: "🦷"; category: 1 }
                    ListElement { emoji: "🦴"; category: 1 }
                    ListElement { emoji: "👀"; category: 1 }
                    ListElement { emoji: "👁️"; category: 1 }
                    ListElement { emoji: "👅"; category: 1 }
                    ListElement { emoji: "👄"; category: 1 }

                    // Animals & Nature
                    ListElement { emoji: "🐶"; category: 2 }
                    ListElement { emoji: "🐱"; category: 2 }
                    ListElement { emoji: "🐭"; category: 2 }
                    ListElement { emoji: "🐹"; category: 2 }
                    ListElement { emoji: "🐰"; category: 2 }
                    ListElement { emoji: "🦊"; category: 2 }
                    ListElement { emoji: "🐻"; category: 2 }
                    ListElement { emoji: "🐼"; category: 2 }
                    ListElement { emoji: "🐨"; category: 2 }
                    ListElement { emoji: "🐯"; category: 2 }
                    ListElement { emoji: "🦁"; category: 2 }
                    ListElement { emoji: "🐮"; category: 2 }
                    ListElement { emoji: "🐷"; category: 2 }
                    ListElement { emoji: "🐸"; category: 2 }
                    ListElement { emoji: "🐵"; category: 2 }
                    ListElement { emoji: "🙈"; category: 2 }
                    ListElement { emoji: "🙉"; category: 2 }
                    ListElement { emoji: "🙊"; category: 2 }
                    ListElement { emoji: "🐒"; category: 2 }
                    ListElement { emoji: "🐔"; category: 2 }
                    ListElement { emoji: "🐧"; category: 2 }
                    ListElement { emoji: "🐦"; category: 2 }
                    ListElement { emoji: "🐤"; category: 2 }
                    ListElement { emoji: "🐣"; category: 2 }
                    ListElement { emoji: "🐥"; category: 2 }
                    ListElement { emoji: "🦆"; category: 2 }
                    ListElement { emoji: "🦅"; category: 2 }
                    ListElement { emoji: "🦉"; category: 2 }
                    ListElement { emoji: "🦇"; category: 2 }
                    ListElement { emoji: "🐺"; category: 2 }
                    ListElement { emoji: "🐗"; category: 2 }
                    ListElement { emoji: "🐴"; category: 2 }
                    ListElement { emoji: "🦄"; category: 2 }
                    ListElement { emoji: "🐝"; category: 2 }
                    ListElement { emoji: "🐛"; category: 2 }
                    ListElement { emoji: "🦋"; category: 2 }
                    ListElement { emoji: "🐌"; category: 2 }
                    ListElement { emoji: "🐞"; category: 2 }
                    ListElement { emoji: "🐜"; category: 2 }
                    ListElement { emoji: "🦟"; category: 2 }
                    ListElement { emoji: "🦗"; category: 2 }
                    ListElement { emoji: "🕷️"; category: 2 }
                    ListElement { emoji: "🕸️"; category: 2 }
                    ListElement { emoji: "🦂"; category: 2 }
                    ListElement { emoji: "🐢"; category: 2 }
                    ListElement { emoji: "🐍"; category: 2 }
                    ListElement { emoji: "🦎"; category: 2 }
                    ListElement { emoji: "🦖"; category: 2 }
                    ListElement { emoji: "🦕"; category: 2 }
                    ListElement { emoji: "🐙"; category: 2 }
                    ListElement { emoji: "🦑"; category: 2 }
                    ListElement { emoji: "🦐"; category: 2 }
                    ListElement { emoji: "🦞"; category: 2 }
                    ListElement { emoji: "🦀"; category: 2 }
                    ListElement { emoji: "🐟"; category: 2 }
                    ListElement { emoji: "🐠"; category: 2 }
                    ListElement { emoji: "🐡"; category: 2 }
                    ListElement { emoji: "🦈"; category: 2 }
                    ListElement { emoji: "🐳"; category: 2 }
                    ListElement { emoji: "🐋"; category: 2 }
                    ListElement { emoji: "🐬"; category: 2 }
                    ListElement { emoji: "🌸"; category: 2 }
                    ListElement { emoji: "🌺"; category: 2 }
                    ListElement { emoji: "🌻"; category: 2 }
                    ListElement { emoji: "🌹"; category: 2 }
                    ListElement { emoji: "🌷"; category: 2 }
                    ListElement { emoji: "🌼"; category: 2 }
                    ListElement { emoji: "🌱"; category: 2 }
                    ListElement { emoji: "🌲"; category: 2 }
                    ListElement { emoji: "🌳"; category: 2 }
                    ListElement { emoji: "🌴"; category: 2 }
                    ListElement { emoji: "🌵"; category: 2 }
                    ListElement { emoji: "🌾"; category: 2 }
                    ListElement { emoji: "🌿"; category: 2 }
                    ListElement { emoji: "☘️"; category: 2 }
                    ListElement { emoji: "🍀"; category: 2 }
                    ListElement { emoji: "🍁"; category: 2 }
                    ListElement { emoji: "🍂"; category: 2 }
                    ListElement { emoji: "🍃"; category: 2 }

                }


                delegate: Item {
                    width: 36
                    height: 36
                    visible: model.category === parent.parent.parent.children[1].currentCategory

                    Rectangle {
                        anchors.fill: parent
                        radius: 6
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: emojiArea.containsMouse ? accentBlue : "transparent" }
                            GradientStop { position: 1.0; color: emojiArea.containsMouse ? Qt.lighter(accentBlue, 1.1) : "transparent" }
                        }

                        Text {
                            anchors.centerIn: parent
                            text: model.emoji
                            font.pixelSize: 20
                        }

                        MouseArea {
                            id: emojiArea
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                messageInput.insert(messageInput.cursorPosition, model.emoji)
                                messageInput.forceActiveFocus()
                                emojiPicker.visible = false
                            }
                        }

                        Behavior on scale {
                            NumberAnimation { duration: 150 }
                        }

                        scale: emojiArea.pressed ? 0.95 : 1.0
                    }
                }
            }
        }

        Behavior on opacity {
            NumberAnimation { duration: 200 }
        }
    }
}
