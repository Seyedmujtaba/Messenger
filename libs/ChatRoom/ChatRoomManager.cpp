#include "ChatRoomManager.h"
#include <sstream>
#include <random>
#include <algorithm>
#include <ctime>
#include <iostream>

// ================== ChatMessage Class Implementation ==================
ChatMessage::ChatMessage(int id, int senderId, const std::string& content,
                         const std::string& attachmentPath, int replyToMessageId)
    : id(id), senderId(senderId), content(content),
      attachmentPath(attachmentPath), replyToMessageId(replyToMessageId)
{
    timestamp = std::time(nullptr);
    readBy.insert(senderId);
}

bool ChatMessage::hasAttachment() const {
    return !attachmentPath.empty();
}

bool ChatMessage::isReply() const {
    return replyToMessageId > 0;
}

bool ChatMessage::isReadBy(int userId) const {
    return readBy.count(userId) > 0;
}

int ChatMessage::getReadCount() const {
    return readBy.size();
}

ChatMessage ChatMessage::fromDatabaseMessage(const ::Message& dbMessage) {
    ChatMessage msg;
    msg.id = dbMessage.id;

    // تبدیل username به userId
    try {
        msg.senderId = std::stoi(dbMessage.sender);
    } catch (...) {
        msg.senderId = -1; // مقدار پیش‌فرض در صورت خطا
    }

    msg.content = dbMessage.content;

    // تبدیل timestamp از string به time_t
    if (!dbMessage.timestamp.empty()) {
        try {
            // فرض می‌کنیم timestamp به صورت عددی است
            msg.timestamp = std::stol(dbMessage.timestamp);
        } catch (...) {
            msg.timestamp = std::time(nullptr);
        }
    } else {
        msg.timestamp = std::time(nullptr);
    }

    // علامت‌گذاری خوانده شده
    if (dbMessage.isRead) {
        msg.readBy.insert(msg.senderId);
    }

    return msg;
}

// ================== ChatRoom Class Implementation ==================
ChatRoom::ChatRoom(int id, const std::string& name, const std::string& bio,
                   const std::string& profileImagePath, bool isPrivate, int creatorId,
                   std::shared_ptr<Database> db)
    : id(id), name(name), bio(bio), profileImagePath(profileImagePath),
      isPrivate(isPrivate), creatorId(creatorId),
      onlyAdminsCanMessage(false), nextMessageId(1), database(db)
{
    admins.insert(creatorId);
    members.insert(creatorId);

    if (!isPrivate) {
        generateInviteLink();
    }

    loadMessagesFromDatabase();
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
    return std::vector<int>(members.begin(), members.end());
}

std::vector<int> ChatRoom::getAdmins() const {
    return std::vector<int>(admins.begin(), admins.end());
}

std::vector<ChatMessage> ChatRoom::getMessages() const {
    return messages;
}

std::vector<int> ChatRoom::getPinnedMessages() const {
    return pinnedMessages;
}

const ChatMessage* ChatRoom::getMessageById(int messageId) const {
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

// ================== Database Integration Methods ==================
bool ChatRoom::syncWithDatabase() {
    return loadMessagesFromDatabase();
}

bool ChatRoom::loadMessagesFromDatabase() {
    if (!database) return false;

    std::string roomName = std::to_string(id);
    auto dbMessages = database->getChatroomMessages(roomName);

    messages.clear();
    for (const auto& dbMsg : dbMessages) {
        messages.push_back(ChatMessage::fromDatabaseMessage(dbMsg));

        if (dbMsg.id >= nextMessageId) {
            nextMessageId = dbMsg.id + 1;
        }
    }

    return true;
}

bool ChatRoom::saveMessageToDatabase(const ChatMessage& message) {
    if (!database) return false;

    std::string senderUsername = std::to_string(message.senderId);
    std::string roomName = std::to_string(id);

    return database->sendMessage(senderUsername, roomName, message.content);
}

bool ChatRoom::saveRoomToDatabase() {
    if (!database) return false;

    std::string roomName = std::to_string(id);
    if (database->createChatroom(name)) {
        for (int memberId : members) {
            std::string username = std::to_string(memberId);
            database->addUserToChatroom(username, name);
        }
        return true;
    }
    return false;
}

bool ChatRoom::addMemberToDatabase(int userId) {
    if (!database) return false;

    std::string username = std::to_string(userId);
    return database->addUserToChatroom(username, name);
}

bool ChatRoom::removeMemberFromDatabase(int userId) {
    // این تابع نیاز به پیاده‌سازی در دیتابیس دارد
    return true;
}

std::string ChatRoom::userIdToUsername(int userId) const {
    return std::to_string(userId);
}

int ChatRoom::usernameToUserId(const std::string& username) const {
    try {
        return std::stoi(username);
    } catch (...) {
        return -1;
    }
}

// ================== Group Information Management ==================
OperationResult ChatRoom::setName(const std::string& newName, int requesterId) {
    if (!hasAdminPrivilege(requesterId)) {
        return {false, ChatRoomError::PERMISSION_DENIED, "Only admins can change group name"};
    }

    std::string oldName = name;
    name = newName;

    // به‌روزرسانی نام در دیتابیس
    if (database) {
        // نیاز به پیاده‌سازی تابع updateChatroomName در دیتابیس
    }

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
            generateInviteLink();
        } else if (isPrivate) {
            inviteLink.clear();
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

    if (!addMemberToDatabase(userId)) {
        members.erase(userId);
        return {false, ChatRoomError::INVALID_REQUEST, "Failed to add member to database"};
    }

    return {true};
}

OperationResult ChatRoom::addMember(int requesterId, int userId) {
    if (!hasAdminPrivilege(requesterId)) {
        return {false, ChatRoomError::PERMISSION_DENIED, "Only admins can add members"};
    }
    if (isMember(userId)) {
        return {false, ChatRoomError::USER_ALREADY_MEMBER, "User is already a member"};
    }
    return addMember(userId);
}

OperationResult ChatRoom::removeMember(int userId) {
    if (!members.count(userId)) {
        return {false, ChatRoomError::USER_NOT_MEMBER, "User is not a member"};
    }
    if (userId == creatorId) {
        return {false, ChatRoomError::CANNOT_REMOVE_OWNER, "Cannot remove group owner"};
    }

    members.erase(userId);
    admins.erase(userId);

    if (!removeMemberFromDatabase(userId)) {
        members.insert(userId);
        return {false, ChatRoomError::INVALID_REQUEST, "Failed to remove member from database"};
    }

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
    return removeMember(userId);
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

    ChatMessage msg(nextMessageId++, senderId, content);
    messages.push_back(msg);

    if (!saveMessageToDatabase(msg)) {
        messages.pop_back();
        nextMessageId--;
        return {false, ChatRoomError::INVALID_REQUEST, "Failed to save message to database"};
    }

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

    ChatMessage msg(nextMessageId++, senderId, content, attachmentPath, replyToMessageId);
    messages.push_back(msg);

    if (!saveMessageToDatabase(msg)) {
        messages.pop_back();
        nextMessageId--;
        return {false, ChatRoomError::INVALID_REQUEST, "Failed to save message to database"};
    }

    return {true};
}

OperationResult ChatRoom::editMessage(int messageId, int senderId, const std::string& newContent) {
    ChatMessage* msg = findMessageById(messageId);
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

    std::string oldContent = msg->content;
    msg->content = newContent;

    // به‌روزرسانی در دیتابیس
    if (database && !database->editMessage(messageId, newContent)) {
        msg->content = oldContent;
        return {false, ChatRoomError::INVALID_REQUEST, "Failed to update message in database"};
    }

    return {true};
}

OperationResult ChatRoom::deleteMessage(int messageId, int requesterId) {
    for (auto it = messages.begin(); it != messages.end(); ++it) {
        if (it->id == messageId) {
            if (it->senderId == requesterId || hasAdminPrivilege(requesterId)) {
                ChatMessage deletedMsg = *it;
                messages.erase(it);

                // حذف از دیتابیس (نیاز به پیاده‌ستی تابع deleteMessage در دیتابیس)
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

    ChatMessage* msg = findMessageById(messageId);
    if (!msg) {
        return {false, ChatRoomError::MESSAGE_NOT_FOUND, "Message not found"};
    }

    msg->readBy.insert(userId);

    // به‌روزرسانی در دیتابیس
    if (database && !database->markMessageAsRead(messageId)) {
        msg->readBy.erase(userId);
        return {false, ChatRoomError::INVALID_REQUEST, "Failed to mark message as read in database"};
    }

    return {true};
}

OperationResult ChatRoom::forwardMessage(int messageId, int forwarderId, ChatRoom& targetRoom) {
    if (!isMember(forwarderId)) {
        return {false, ChatRoomError::NOT_MEMBER, "User is not member of source group"};
    }

    if (!targetRoom.isMember(forwarderId)) {
        return {false, ChatRoomError::FORWARD_NOT_MEMBER, "User is not member of target group"};
    }

    const ChatMessage* msg = getMessageById(messageId);
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

std::vector<ChatMessage> ChatRoom::getMessagesWithReplies() const {
    std::vector<ChatMessage> messagesWithReplies;
    for (const auto& msg : messages) {
        if (msg.isReply()) {
            messagesWithReplies.push_back(msg);
        }
    }
    return messagesWithReplies;
}

std::vector<ChatMessage> ChatRoom::getUnreadMessages(int userId) const {
    std::vector<ChatMessage> unreadMessages;
    if (!isMember(userId)) {
        return unreadMessages;
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

OperationResult ChatRoom::searchMessages(const std::string& keyword, std::vector<ChatMessage>& results) const {
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

ChatMessage* ChatRoom::findMessageById(int messageId) {
    for (auto& msg : messages) {
        if (msg.id == messageId) {
            return &msg;
        }
    }
    return nullptr;
}

// ================== ChatRoomManager Class Implementation ==================
ChatRoomManager::ChatRoomManager(std::shared_ptr<Database> db)
    : nextRoomId(1), database(db)
{
    loadAllRoomsFromDatabase();
}

// ================== Database Integration Methods ==================
bool ChatRoomManager::loadAllRoomsFromDatabase() {
    if (!database) return false;

    // ابتدا کاربران را لود کنید
    auto userChats = database->getUserChats("1"); // فرضی

    // سپس چت روم‌ها را ایجاد کنید
    // این بخش نیاز به پیاده‌سازی کامل‌تر دارد

    return true;
}

bool ChatRoomManager::registerUser(const std::string& username, const std::string& password) {
    if (!database) return false;
    return database->createAccount(username, password);
}

bool ChatRoomManager::authenticateUser(const std::string& username, const std::string& password) {
    if (!database) return false;
    return database->login(username, password);
}

int ChatRoomManager::getUserIdFromUsername(const std::string& username) const {
    try {
        return std::stoi(username);
    } catch (...) {
        return -1;
    }
}

std::string ChatRoomManager::getUsernameFromUserId(int userId) const {
    return std::to_string(userId);
}

// ================== Room Management ==================
OperationResult ChatRoomManager::createRoom(const std::string& name, const std::string& bio,
                                           const std::string& profileImagePath, bool isPrivate,
                                           int creatorId, ChatRoom*& outRoom) {
    if (name.empty()) {
        return {false, ChatRoomError::INVALID_REQUEST, "Room name cannot be empty"};
    }

    ChatRoom room(nextRoomId, name, bio, profileImagePath, isPrivate, creatorId, database);
    auto result = chatRooms.emplace(nextRoomId, std::move(room));
    outRoom = &result.first->second;

    if (!outRoom->saveRoomToDatabase()) {
        chatRooms.erase(nextRoomId);
        return {false, ChatRoomError::INVALID_REQUEST, "Failed to save room to database"};
    }

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

// ================== Member Management ==================
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

// ================== Statistics ==================
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
