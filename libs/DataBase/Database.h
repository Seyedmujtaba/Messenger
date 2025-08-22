#ifndef DATABASE_H
#define DATABASE_H
#include <string>
#include <vector>
#include <utility> // for std::pair
#include <optional>

// Represents a single message
struct Message {
    int id;
    std::string sender;
    std::string receiver; // Can be a username or chatroom name
    std::string content;
    std::string timestamp;
    bool isRead;
    bool isEdited;
};

// Represents a chat (either direct message or chatroom)
struct Chat {
    std::string name;        // Username for DMs, chatroom name for groups
    std::string type;        // "direct" or "chatroom"
    std::string lastMessage; // Content of most recent message
    std::string lastMessageTime;
    int unreadCount;
    bool isActive;          // Whether chat is still active/accessible
};

class Database {
public:
    Database(const std::string& dbPath); // Constructor (open/init DB)
    ~Database();                         // Destructor (close DB)
    
    // User account management
    bool createAccount(const std::string& username, const std::string& password);
    bool login(const std::string& username, const std::string& password);
    
    // Chatroom management
    bool createChatroom(const std::string& chatroomName);
    bool addUserToChatroom(const std::string& username, const std::string& chatroomName);
    
    // Message handling
    bool sendMessage(const std::string& sender, const std::string& receiver, const std::string& content);
    bool editMessage(int messageId, const std::string& newContent);
    bool markMessageAsRead(int messageId);
    
    // Message queries
    std::vector<Message> getMessageHistory(const std::string& user1, const std::string& user2);
    std::vector<Message> getChatroomMessages(const std::string& chatroomName);
    int getTotalMessagesSent(const std::string& username);
    int getUnreadMessageCount(const std::string& username);
    
    // Chat overview
    std::vector<Chat> getUserChats(const std::string& username);
    
private:
    void connect(const std::string& dbPath);
    void disconnect();
    // Optional: internal helpers for query execution
    void initializeSchema(); // Called during construction to ensure DB schema exists
    // Your DB connection object (placeholder, replace with actual DB object, e.g. SQLite3* db)
    void* dbConnection;
};

#endif // DATABASE_H