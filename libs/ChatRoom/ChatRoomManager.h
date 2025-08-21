#ifndef CHATROOM_H
#define CHATROOM_H

#include <string>
#include <vector>
#include <set>
#include <map>
#include <ctime>

enum class ChatRoomError {
    SUCCESS,                    // Operation completed successfully
    NOT_MEMBER,                 // User is not a member of the room
    NOT_ADMIN,                  // User does not have admin privileges
    NOT_OWNER,                  // User is not the room owner
    USER_ALREADY_MEMBER,        // User is already a member
    USER_NOT_MEMBER,            // User is not a member
    CANNOT_REMOVE_OWNER,        // Attempt to remove room owner
    INVALID_INVITE_LINK,        // Invite link is invalid or expired
    PERMISSION_DENIED,          // User lacks required permissions
    ROOM_NOT_FOUND,             // Requested room does not exist
    MESSAGE_TOO_LONG,           // Message exceeds maximum length
    MESSAGE_EMPTY,              // Message content is empty
    MESSAGE_NOT_FOUND,          // Message does not exist
    EDIT_TIMEOUT,               // Message edit time window expired
    INVALID_REQUEST,            // Malformed or invalid request
    FORWARD_NOT_MEMBER,         // User not member of target room for forwarding
    FORWARD_MESSAGE_NOT_FOUND,  // Message to forward not found
    FORWARD_PERMISSION_DENIED,  // Permission denied for forwarding
    MESSAGE_ALREADY_PINNED,     // Message is already pinned
    ATTACHMENT_TOO_LARGE,       // Attachment exceeds size limit
    INVALID_ATTACHMENT_TYPE,    // Attachment type not allowed
    REPLY_MESSAGE_NOT_FOUND     // Message being replied to not found
};

struct OperationResult {
    bool success;               // True if operation succeeded
    ChatRoomError error;        // Error code if operation failed
    std::string message;        // Human-readable error/success message

    OperationResult(bool success = true,
                   ChatRoomError error = ChatRoomError::SUCCESS,
                   const std::string& message = "")
        : success(success), error(error), message(message) {}
};

struct Message {
    int id;                     // Unique message identifier
    int senderId;               // ID of user who sent the message
    std::string content;        // Text content of the message
    std::string attachmentPath; // Path to attached file (optional)
    int replyToMessageId;       // ID of message being replied to (optional)
    std::time_t timestamp;      // Unix timestamp of message creation
    std::set<int> readBy;       // Set of user IDs who have read the message

    Message(int id, int senderId, const std::string& content,
            const std::string& attachmentPath = "",
            int replyToMessageId = -1);

    // ============ Utility Methods ============
    bool isReadBy(int userId) const;
    int getReadCount() const;
    bool hasAttachment() const;
    bool isReply() const;
};

class ChatRoom {
private:
    // ============ Room Properties ============
    int id;                     // Unique room identifier
    std::string name;           // Room display name
    std::string bio;            // Room description
    std::string profileImagePath; // Room avatar image path
    bool isPrivate;             // Privacy setting (true = private)
    std::string inviteLink;     // Unique invite link for public rooms
    int creatorId;              // ID of user who created the room

    // ============ Member Management ============
    std::set<int> members;      // Set of member user IDs
    std::set<int> admins;       // Set of admin user IDs
    bool onlyAdminsCanMessage;  // Restriction setting for messaging

    // ============ Message Management ============
    std::vector<Message> messages;      // List of all messages
    std::vector<int> pinnedMessages;    // List of pinned message IDs
    int nextMessageId;          // Next available message ID

public:
    ChatRoom(int id, const std::string& name, const std::string& bio,
             const std::string& profileImagePath, bool isPrivate, int creatorId);

    // ================= Getters =================

    int getId() const;
    std::string getName() const;
    std::string getBio() const;
    std::string getProfileImagePath() const;
    bool getIsPrivate() const;
    std::string getInviteLink() const;
    int getCreatorId() const;
    std::vector<int> getMembers() const;
    std::vector<int> getAdmins() const;
    std::vector<Message> getMessages() const;
    std::vector<int> getPinnedMessages() const;
    bool getOnlyAdminsCanMessage() const;
    const Message* getMessageById(int messageId) const;

    // ================= Group Information Management =================
    OperationResult setName(const std::string& newName, int requesterId);
    OperationResult setBio(const std::string& newBio, int requesterId);
    OperationResult setProfileImage(const std::string& newPath, int requesterId);
    OperationResult setPrivacy(bool newPrivacy, int requesterId);
    OperationResult editGroupInfo(int requesterId, const std::string& newName, const std::string& newBio);

    // ================= Member Management =================
    OperationResult addMember(int userId);
    OperationResult addMember(int requesterId, int userId);
    OperationResult removeMember(int userId);
    OperationResult removeMember(int requesterId, int userId);

    bool isMember(int userId) const;
    bool isOwner(int userId) const;

    // ================= Admin Management =================
    OperationResult addAdmin(int userId, int requesterId);
    OperationResult addAdmin(int requesterId, int userId);
    OperationResult removeAdmin(int userId, int requesterId);
    OperationResult removeAdmin(int requesterId, int userId);

    bool isAdmin(int userId) const;

    // ================= Message Management =================
    OperationResult setOnlyAdminsCanMessage(bool value, int requesterId);
    OperationResult sendMessage(int senderId, const std::string& content);
    OperationResult sendMessage(int senderId, const std::string& content,
                               const std::string& attachmentPath,
                               int replyToMessageId);
    OperationResult editMessage(int messageId, int senderId, const std::string& newContent);
    OperationResult deleteMessage(int messageId, int requesterId);
    OperationResult markMessageAsRead(int messageId, int userId);
    OperationResult forwardMessage(int messageId, int forwarderId, ChatRoom& targetRoom);
    OperationResult pinMessage(int userId, int messageId);
    OperationResult searchMessages(const std::string& keyword, std::vector<Message>& results) const;

    // ================= Utilities and Statistics =================
    std::vector<Message> getMessagesWithReplies() const;
    std::vector<Message> getUnreadMessages(int userId) const;
    int getUnreadCount(int userId) const;
    int getTotalMessages() const;
    int getActiveMembersCount() const;

private:
    // ================= Private Methods =================
    void generateInviteLink();
    bool hasAdminPrivilege(int userId) const;
    Message* findMessageById(int messageId);
};
class ChatRoomManager {
private:
    std::map<int, ChatRoom> chatRooms;  // Map of room ID to ChatRoom objects
    int nextRoomId;                     // Next available room ID

public:
    ChatRoomManager();

    // ================= Room Management =================
    OperationResult createRoom(const std::string& name, const std::string& bio,
                              const std::string& profileImagePath, bool isPrivate,
                              int creatorId, ChatRoom*& outRoom);

    OperationResult deleteRoom(int roomId, int requesterId);

    // ================= Room Search =================
    ChatRoom* getRoomById(int roomId);
    const ChatRoom* getRoomById(int roomId) const;
    ChatRoom* getRoomByLink(const std::string& inviteLink);
    std::vector<int> getUserRooms(int userId) const;
    std::vector<int> getAllRoomIds() const;

    // ================= Member Management =================
    OperationResult addMemberToRoom(int roomId, int userId, int requesterId = -1);
    OperationResult addMemberByLink(const std::string& inviteLink, int userId);
    OperationResult removeMemberFromRoom(int roomId, int userId, int requesterId = -1);

    // ================= Statistics =================
    int getTotalRoomsCount() const;
    int getUserRoomCount(int userId) const;

private:
    bool isValidRoomName(const std::string& name) const;
};

#endif // CHATROOM_H
