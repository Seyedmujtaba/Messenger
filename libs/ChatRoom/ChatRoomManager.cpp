#include "ChatRoom.h"
#include <sstream>
#include <random>
#include <algorithm>
#include <ctime>

// ================== Message Class Implementation ==================
Message::Message(int id, int senderId, const std::string& content,
                 const std::string& attachmentPath, int replyToMessageId)
    : id(id), senderId(senderId), content(content),
      attachmentPath(attachmentPath), replyToMessageId(replyToMessageId)
{
    timestamp = std::time(nullptr);
    readBy.insert(senderId); // Sender automatically marks message as read
}

bool Message::hasAttachment() const {
    return !attachmentPath.empty();
}

bool Message::isReply() const {
    return replyToMessageId > 0;
}

bool Message::isReadBy(int userId) const {
    return readBy.count(userId) > 0;
}

int Message::getReadCount() const {
    return readBy.size();
}

// ================== ChatRoom Class Implementation ==================
ChatRoom::ChatRoom(int id, const std::string& name, const std::string& bio,
                   const std::string& profileImagePath, bool isPrivate, int creatorId)
    : id(id), name(name), bio(bio), profileImagePath(profileImagePath),
      isPrivate(isPrivate), creatorId(creatorId),
      onlyAdminsCanMessage(false), nextMessageId(1)
{
    admins.insert(creatorId);    // Creator becomes admin
    members.insert(creatorId);   // Creator becomes member

    if (!isPrivate) {
        generateInviteLink();    // Generate invite link for public rooms
    }
}

// ================== Getter Methods ==================
int ChatRoom::getId() const { return id; }
std::string ChatRoom::getName() const { return name; }
std::string ChatRoom::getBio() const { return bio; }
std::string ChatRoom::getProfileImagePath() const { return profileImagePath; }
bool ChatRoom::getIsPrivate() const { return isPrivate; }
std::string ChatRoom::getInviteLink() const { return inviteLink; }
int ChatRoom::getCreatorId() const { return creatorId; }
std::vector<int> ChatRoom::getMembers() const {
    return {members.begin(), members.end()};
}
std::vector<int> ChatRoom::getAdmins() const {
    return {admins.begin(), admins.end()};
}
std::vector<Message> ChatRoom::getMessages() const {
    return messages;
}
std::vector<int> ChatRoom::getPinnedMessages() const {
    return pinnedMessages;
}

const Message* ChatRoom::getMessageById(int messageId) const {
    for (const auto& msg : messages) {
        if (msg.id == messageId) {
            return &msg;
        }
    }
    return nullptr;
}

bool ChatRoom::getOnlyAdminsCanMessage() const {
    return onlyAdminsCanMessage;
}

// ================== Group Information Management ==================
OperationResult ChatRoom::setName(const std::string& newName, int requesterId) {
    if (!hasAdminPrivilege(requesterId)) {
        return {false, ChatRoomError::PERMISSION_DENIED, "Only admins can change group name"};
    }
    name = newName;
    return {true};
}

OperationResult ChatRoom::setBio(const std::string& newBio, int requesterId) {
    if (!hasAdminPrivilege(requesterId)) {
        return {false, ChatRoomError::PERMISSION_DENIED, "Only admins can change group bio"};
    }
    bio = newBio;
    return {true};
}

OperationResult ChatRoom::setProfileImage(const std::string& newPath, int requesterId) {
    if (!hasAdminPrivilege(requesterId)) {
        return {false, ChatRoomError::PERMISSION_DENIED, "Only admins can change group image"};
    }
    profileImagePath = newPath;
    return {true};
}

OperationResult ChatRoom::setPrivacy(bool newPrivacy, int requesterId) {
    if (!hasAdminPrivilege(requesterId)) {
        return {false, ChatRoomError::PERMISSION_DENIED, "Only admins can change group privacy"};
    }

    if (isPrivate != newPrivacy) {
        isPrivate = newPrivacy;
        if (!isPrivate && inviteLink.empty()) {
            generateInviteLink();    // Generate link when making public
        } else if (isPrivate) {
            inviteLink.clear();      // Remove link when making private
        }
    }
    return {true};
}

// ================== Member Management ==================
OperationResult ChatRoom::addMember(int userId) {
    if (members.count(userId)) {
        return {false, ChatRoomError::USER_ALREADY_MEMBER, "User is already a member"};
    }
    members.insert(userId);
    return {true};
}

OperationResult ChatRoom::addMember(int requesterId, int userId) {
    if (!hasAdminPrivilege(requesterId)) {
        return {false, ChatRoomError::PERMISSION_DENIED, "Only admins can add members"};
    }
    if (isMember(userId)) {
        return {false, ChatRoomError::USER_ALREADY_MEMBER, "User is already a member"};
    }
    members.insert(userId);
    return {true};
}

OperationResult ChatRoom::removeMember(int userId) {
    if (!members.count(userId)) {
        return {false, ChatRoomError::USER_NOT_MEMBER, "User is not a member"};
    }
    if (userId == creatorId) {
        return {false, ChatRoomError::CANNOT_REMOVE_OWNER, "Cannot remove group owner"};
    }
    members.erase(userId);
    admins.erase(userId); // Also remove from admins if they were admin
    return {true};
}

OperationResult ChatRoom::removeMember(int requesterId, int userId) {
    if (!hasAdminPrivilege(requesterId)) {
        return {false, ChatRoomError::PERMISSION_DENIED, "Only admins can remove members"};
    }
    if (!isMember(userId)) {
        return {false, ChatRoomError::USER_NOT_MEMBER, "User is not a member"};
    }
    if (isOwner(userId)) {
        return {false, ChatRoomError::CANNOT_REMOVE_OWNER, "Cannot remove group owner"};
    }
    members.erase(userId);
    admins.erase(userId);
    return {true};
}

bool ChatRoom::isMember(int userId) const {
    return members.count(userId) > 0;
}

bool ChatRoom::isOwner(int userId) const {
    return userId == creatorId;
}

// ================== Admin Management ==================
OperationResult ChatRoom::addAdmin(int userId, int requesterId) {
    if (!hasAdminPrivilege(requesterId)) {
        return {false, ChatRoomError::PERMISSION_DENIED, "Only admins can add new admins"};
    }
    if (!isMember(userId)) {
        return {false, ChatRoomError::USER_NOT_MEMBER, "User is not a member"};
    }
    admins.insert(userId);
    return {true};
}
OperationResult ChatRoom::addAdmin(int requesterId, int userId) {
    if (!isOwner(requesterId)) {
        return {false, ChatRoomError::PERMISSION_DENIED, "Only owner can add admins"};
    }
    if (!isMember(userId)) {
        return {false, ChatRoomError::USER_NOT_MEMBER, "User is not a member"};
    }
    admins.insert(userId);
    return {true};
}
OperationResult ChatRoom::removeAdmin(int userId, int requesterId) {
    if (!hasAdminPrivilege(requesterId)) {
        return {false, ChatRoomError::PERMISSION_DENIED, "Only admins can remove admins"};
    }
    if (userId == creatorId) {
        return {false, ChatRoomError::CANNOT_REMOVE_OWNER, "Cannot remove owner admin status"};
    }
    if (!admins.count(userId)) {
        return {false, ChatRoomError::NOT_ADMIN, "User is not an admin"};
    }
    admins.erase(userId);
    return {true};
}
OperationResult ChatRoom::removeAdmin(int requesterId, int userId) {
    if (!isOwner(requesterId)) {
        return {false, ChatRoomError::PERMISSION_DENIED, "Only owner can remove admins"};
    }
    if (isOwner(userId)) {
        return {false, ChatRoomError::CANNOT_REMOVE_OWNER, "Cannot remove owner admin status"};
    }
    if (!isAdmin(userId)) {
        return {false, ChatRoomError::NOT_ADMIN, "User is not an admin"};
    }
    admins.erase(userId);
    return {true};
}
bool ChatRoom::isAdmin(int userId) const {
    return admins.count(userId) > 0;
}

// ================== Message Settings ==================
OperationResult ChatRoom::setOnlyAdminsCanMessage(bool value, int requesterId) {
    if (!hasAdminPrivilege(requesterId)) {
        return {false, ChatRoomError::PERMISSION_DENIED, "Only admins can change this setting"};
    }
    onlyAdminsCanMessage = value;
    return {true};
}

// ================== Message Operations ==================
OperationResult ChatRoom::sendMessage(int senderId, const std::string& content) {
    if (!isMember(senderId)) {
        return {false, ChatRoomError::NOT_MEMBER, "User is not a member"};
    }
    if (onlyAdminsCanMessage && !isAdmin(senderId)) {
        return {false, ChatRoomError::NOT_ADMIN, "Only admins can send messages"};
    }
    if (content.empty()) {
        return {false, ChatRoomError::MESSAGE_EMPTY, "Message cannot be empty"};
    }
    if (content.length() > 1000) {
        return {false, ChatRoomError::MESSAGE_TOO_LONG, "Message too long (max 1000 characters)"};
    }

    Message msg(nextMessageId++, senderId, content);
    messages.push_back(msg);
    return {true};
}
OperationResult ChatRoom::sendMessage(int senderId, const std::string& content,
                                     const std::string& attachmentPath,
                                     int replyToMessageId)
{
    if (!isMember(senderId)) {
        return {false, ChatRoomError::NOT_MEMBER, "User is not a member"};
    }
    if (onlyAdminsCanMessage && !isAdmin(senderId)) {
        return {false, ChatRoomError::NOT_ADMIN, "Only admins can send messages"};
    }
    if (content.empty() && attachmentPath.empty()) {
        return {false, ChatRoomError::MESSAGE_EMPTY, "Message cannot be empty"};
    }
    if (content.length() > 1000) {
        return {false, ChatRoomError::MESSAGE_TOO_LONG, "Message too long (max 1000 characters)"};
    }
    if (replyToMessageId > 0 && !getMessageById(replyToMessageId)) {
        return {false, ChatRoomError::REPLY_MESSAGE_NOT_FOUND, "Reply message not found"};
    }

    Message msg(nextMessageId++, senderId, content, attachmentPath, replyToMessageId);
    messages.push_back(msg);
    return {true};
}
OperationResult ChatRoom::editMessage(int messageId, int senderId, const std::string& newContent) {
    Message* msg = findMessageById(messageId);
    if (!msg) {
        return {false, ChatRoomError::MESSAGE_NOT_FOUND, "Message not found"};
    }
    if (msg->senderId != senderId) {
        return {false, ChatRoomError::PERMISSION_DENIED, "Only sender can edit message"};
    }

    std::time_t now = std::time(nullptr);
    double diff = difftime(now, msg->timestamp);
    if (diff > 7 * 24 * 60 * 60) {
        return {false, ChatRoomError::EDIT_TIMEOUT, "Edit timeout (1 week expired)"};
    }
    if (newContent.empty()) {
        return {false, ChatRoomError::MESSAGE_EMPTY, "Message cannot be empty"};
    }
    if (newContent.length() > 1000) {
        return {false, ChatRoomError::MESSAGE_TOO_LONG, "Message too long (max 1000 characters)"};
    }

    msg->content = newContent;
    return {true};
}
OperationResult ChatRoom::deleteMessage(int messageId, int requesterId) {
    for (auto it = messages.begin(); it != messages.end(); ++it) {
        if (it->id == messageId) {
            if (it->senderId == requesterId || hasAdminPrivilege(requesterId)) {
                messages.erase(it);
                return {true};
            } else {
                return {false, ChatRoomError::PERMISSION_DENIED, "Only sender or admin can delete message"};
            }
        }
    }
    return {false, ChatRoomError::MESSAGE_NOT_FOUND, "Message not found"};
}
OperationResult ChatRoom::markMessageAsRead(int messageId, int userId) {
    if (!isMember(userId)) {
        return {false, ChatRoomError::NOT_MEMBER, "User is not a member"};
    }

    Message* msg = findMessageById(messageId);
    if (!msg) {
        return {false, ChatRoomError::MESSAGE_NOT_FOUND, "Message not found"};
    }

    msg->readBy.insert(userId);
    return {true};
}
OperationResult ChatRoom::forwardMessage(int messageId, int forwarderId, ChatRoom& targetRoom) {
    if (!isMember(forwarderId)) {
        return {false, ChatRoomError::NOT_MEMBER, "User is not member of source group"};
    }

    if (!targetRoom.isMember(forwarderId)) {
        return {false, ChatRoomError::FORWARD_NOT_MEMBER, "User is not member of target group"};
    }

    const Message* msg = getMessageById(messageId);
    if (!msg) {
        return {false, ChatRoomError::FORWARD_MESSAGE_NOT_FOUND, "Message not found for forwarding"};
    }

    std::string forwardContent = "[Forwarded from " + getName() + "] " + msg->content;
    return targetRoom.sendMessage(forwarderId, forwardContent);
}

// ================== Statistics and Analytics ==================
int ChatRoom::getUnreadCount(int userId) const {
    if (!isMember(userId)) return 0;

    int count = 0;
    for (const auto& msg : messages) {
        if (!msg.isReadBy(userId)) {
            count++;
        }
    }
    return count;
}
int ChatRoom::getTotalMessages() const {
    return messages.size();
}
int ChatRoom::getActiveMembersCount() const {
    return members.size();
}
std::vector<Message> ChatRoom::getMessagesWithReplies() const {
    std::vector<Message> messagesWithReplies;
    for (const auto& msg : messages) {
        if (msg.isReply()) {
            messagesWithReplies.push_back(msg);
        }
    }
    return messagesWithReplies;
}
std::vector<Message> ChatRoom::getUnreadMessages(int userId) const {
    std::vector<Message> unreadMessages;
    if (!isMember(userId)) {
        return unreadMessages; // Return empty list for non-members
    }

    for (const auto& msg : messages) {
        if (!msg.isReadBy(userId)) {
            unreadMessages.push_back(msg);
        }
    }
    return unreadMessages;
}

// ================== Group Management ==================
OperationResult ChatRoom::editGroupInfo(int requesterId, const std::string& newName, const std::string& newBio) {
    if (!hasAdminPrivilege(requesterId)) {
        return {false, ChatRoomError::PERMISSION_DENIED, "Only admins can edit group info"};
    }
    name = newName;
    bio = newBio;
    return {true};
}

// ================== Advanced Features ==================
OperationResult ChatRoom::pinMessage(int userId, int messageId) {
    if (!isMember(userId)) {
        return {false, ChatRoomError::NOT_MEMBER, "Only members can pin messages"};
    }

    if (std::find(pinnedMessages.begin(), pinnedMessages.end(), messageId) != pinnedMessages.end()) {
        return {false, ChatRoomError::MESSAGE_ALREADY_PINNED, "Message is already pinned"};
    }

    if (!getMessageById(messageId)) {
        return {false, ChatRoomError::MESSAGE_NOT_FOUND, "Message not found"};
    }

    pinnedMessages.push_back(messageId);
    return {true};
}
OperationResult ChatRoom::searchMessages(const std::string& keyword, std::vector<Message>& results) const {
    results.clear();
    for (const auto& msg : messages) {
        if (msg.content.find(keyword) != std::string::npos) {
            results.push_back(msg);
        }
    }
    return {true};
}

// ================== Private Methods ==================
void ChatRoom::generateInviteLink() {
    std::stringstream ss;
    ss << "chatapp://join/" << id << "/";

    static std::mt19937 rng(std::random_device{}());
    static std::uniform_int_distribution<int> dist(0, 35);
    const char* chars = "0123456789abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < 12; i++) {
        ss << chars[dist(rng)];
    }

    inviteLink = ss.str();
}
bool ChatRoom::hasAdminPrivilege(int userId) const {
    return isAdmin(userId) || isOwner(userId);
}
Message* ChatRoom::findMessageById(int messageId) {
    for (auto& msg : messages) {
        if (msg.id == messageId) {
            return &msg;
        }
    }
    return nullptr;
}

// ================== ChatRoomManager Class Implementation ==================
ChatRoomManager::ChatRoomManager() : nextRoomId(1) {}

OperationResult ChatRoomManager::createRoom(const std::string& name, const std::string& bio,
                                           const std::string& profileImagePath, bool isPrivate,
                                           int creatorId, ChatRoom*& outRoom) {
    if (name.empty()) {
        return {false, ChatRoomError::INVALID_REQUEST, "Room name cannot be empty"};
    }

    ChatRoom room(nextRoomId, name, bio, profileImagePath, isPrivate, creatorId);
    auto result = chatRooms.emplace(nextRoomId, std::move(room));
    outRoom = &result.first->second;
    nextRoomId++;

    return {true};
}
OperationResult ChatRoomManager::deleteRoom(int roomId, int requesterId) {
    auto it = chatRooms.find(roomId);
    if (it == chatRooms.end()) {
        return {false, ChatRoomError::ROOM_NOT_FOUND, "Room not found"};
    }
    if (!it->second.isOwner(requesterId)) {
        return {false, ChatRoomError::PERMISSION_DENIED, "Only owner can delete room"};
    }

    chatRooms.erase(it);
    return {true};
}
ChatRoom* ChatRoomManager::getRoomById(int roomId) {
    auto it = chatRooms.find(roomId);
    return it != chatRooms.end() ? &it->second : nullptr;
}
const ChatRoom* ChatRoomManager::getRoomById(int roomId) const {
    auto it = chatRooms.find(roomId);
    return it != chatRooms.end() ? &it->second : nullptr;
}
ChatRoom* ChatRoomManager::getRoomByLink(const std::string& inviteLink) {
    for (auto& pair : chatRooms) {
        if (pair.second.getInviteLink() == inviteLink) {
            return &pair.second;
        }
    }
    return nullptr;
}
OperationResult ChatRoomManager::addMemberToRoom(int roomId, int userId, int requesterId) {
    auto* room = getRoomById(roomId);
    if (!room) {
        return {false, ChatRoomError::ROOM_NOT_FOUND, "Room not found"};
    }

    if (room->getIsPrivate() && requesterId >= 0 && !room->isAdmin(requesterId)) {
        return {false, ChatRoomError::PERMISSION_DENIED, "Only admins can add members to private groups"};
    }

    return room->addMember(userId);
}
OperationResult ChatRoomManager::addMemberByLink(const std::string& inviteLink, int userId) {
    auto* room = getRoomByLink(inviteLink);
    if (!room) {
        return {false, ChatRoomError::INVALID_INVITE_LINK, "Invalid invite link"};
    }
    if (room->getIsPrivate()) {
        return {false, ChatRoomError::PERMISSION_DENIED, "Cannot join private group with link"};
    }

    return room->addMember(userId);
}
OperationResult ChatRoomManager::removeMemberFromRoom(int roomId, int userId, int requesterId) {
    auto* room = getRoomById(roomId);
    if (!room) {
        return {false, ChatRoomError::ROOM_NOT_FOUND, "Room not found"};
    }

    if (requesterId >= 0 && requesterId != userId && !room->isAdmin(requesterId)) {
        return {false, ChatRoomError::PERMISSION_DENIED, "Only admins can remove other members"};
    }

    return room->removeMember(userId);
}
std::vector<int> ChatRoomManager::getAllRoomIds() const {
    std::vector<int> ids;
    for (const auto& pair : chatRooms) {
        ids.push_back(pair.first);
    }
    return ids;
}
std::vector<int> ChatRoomManager::getUserRooms(int userId) const {
    std::vector<int> userRooms;
    for (const auto& pair : chatRooms) {
        if (pair.second.isMember(userId)) {
            userRooms.push_back(pair.first);
        }
    }
    return userRooms;
}
int ChatRoomManager::getTotalRoomsCount() const {
    return chatRooms.size();
}
int ChatRoomManager::getUserRoomCount(int userId) const {
    return getUserRooms(userId).size();
}
bool ChatRoomManager::isValidRoomName(const std::string& name) const {
    return !name.empty() && name.length() <= 100;
}
