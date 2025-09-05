#include "Database.h"
#include <sqlite3.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <iomanip>

Database::Database(const std::string& dbPath) : dbConnection(nullptr) {
    connect(dbPath);
    if (dbConnection) {
        initializeSchema();
    }
}

Database::~Database() {
    disconnect();
}

void Database::connect(const std::string& dbPath) {
    sqlite3* db = nullptr;
    int rc = sqlite3_open(dbPath.c_str(), &db);
    
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        dbConnection = nullptr;
    } else {
        dbConnection = db;
        std::cout << "Database opened successfully" << std::endl;
    }
}

void Database::disconnect() {
    if (dbConnection) {
        sqlite3_close(static_cast<sqlite3*>(dbConnection));
        dbConnection = nullptr;
    }
}

void Database::initializeSchema() {
    sqlite3* db = static_cast<sqlite3*>(dbConnection);
    
    const char* createTables = R"(
        -- Users table
        CREATE TABLE IF NOT EXISTS users (
            username TEXT PRIMARY KEY,
            password_hash TEXT NOT NULL,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        );
        
        -- Chatrooms table
        CREATE TABLE IF NOT EXISTS chatrooms (
            name TEXT PRIMARY KEY,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        );
        
        -- User-Chatroom membership table
        CREATE TABLE IF NOT EXISTS chatroom_members (
            username TEXT,
            chatroom_name TEXT,
            joined_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            PRIMARY KEY (username, chatroom_name),
            FOREIGN KEY (username) REFERENCES users(username),
            FOREIGN KEY (chatroom_name) REFERENCES chatrooms(name)
        );
        
        -- Messages table
        CREATE TABLE IF NOT EXISTS messages (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            sender TEXT NOT NULL,
            receiver TEXT NOT NULL,
            content TEXT NOT NULL,
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
            is_read BOOLEAN DEFAULT FALSE,
            is_edited BOOLEAN DEFAULT FALSE,
            FOREIGN KEY (sender) REFERENCES users(username)
        );
        
        -- Indexes for better performance
        CREATE INDEX IF NOT EXISTS idx_messages_sender ON messages(sender);
        CREATE INDEX IF NOT EXISTS idx_messages_receiver ON messages(receiver);
        CREATE INDEX IF NOT EXISTS idx_messages_timestamp ON messages(timestamp);
    )";
    
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, createTables, nullptr, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
}

// Simple hash function (in production, use proper password hashing like bcrypt)
std::string hashPassword(const std::string& password) {
    std::hash<std::string> hasher;
    return std::to_string(hasher(password));
}

bool Database::createAccount(const std::string& username, const std::string& password) {
    sqlite3* db = static_cast<sqlite3*>(dbConnection);
    if (!db) return false;
    
    std::string hashedPassword = hashPassword(password);
    
    const char* sql = "INSERT INTO users (username, password_hash) VALUES (?, ?)";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return false;
    
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, hashedPassword.c_str(), -1, SQLITE_STATIC);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE;
}

bool Database::login(const std::string& username, const std::string& password) {
    sqlite3* db = static_cast<sqlite3*>(dbConnection);
    if (!db) return false;
    
    std::string hashedPassword = hashPassword(password);
    
    const char* sql = "SELECT username FROM users WHERE username = ? AND password_hash = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return false;
    
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, hashedPassword.c_str(), -1, SQLITE_STATIC);
    
    rc = sqlite3_step(stmt);
    bool found = (rc == SQLITE_ROW);
    
    sqlite3_finalize(stmt);
    return found;
}

bool Database::createChatroom(const std::string& chatroomName) {
    sqlite3* db = static_cast<sqlite3*>(dbConnection);
    if (!db) return false;
    
    const char* sql = "INSERT INTO chatrooms (name) VALUES (?)";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return false;
    
    sqlite3_bind_text(stmt, 1, chatroomName.c_str(), -1, SQLITE_STATIC);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE;
}

bool Database::addUserToChatroom(const std::string& username, const std::string& chatroomName) {
    sqlite3* db = static_cast<sqlite3*>(dbConnection);
    if (!db) return false;
    
    const char* sql = "INSERT INTO chatroom_members (username, chatroom_name) VALUES (?, ?)";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return false;
    
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, chatroomName.c_str(), -1, SQLITE_STATIC);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE;
}

bool Database::sendMessage(const std::string& sender, const std::string& receiver, const std::string& content) {
    sqlite3* db = static_cast<sqlite3*>(dbConnection);
    if (!db) return false;
    
    const char* sql = "INSERT INTO messages (sender, receiver, content) VALUES (?, ?, ?)";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return false;
    
    sqlite3_bind_text(stmt, 1, sender.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, receiver.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, content.c_str(), -1, SQLITE_STATIC);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE;
}

bool Database::editMessage(int messageId, const std::string& newContent) {
    sqlite3* db = static_cast<sqlite3*>(dbConnection);
    if (!db) return false;
    
    const char* sql = "UPDATE messages SET content = ?, is_edited = TRUE WHERE id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return false;
    
    sqlite3_bind_text(stmt, 1, newContent.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, messageId);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE && sqlite3_changes(db) > 0;
}

bool Database::markMessageAsRead(int messageId) {
    sqlite3* db = static_cast<sqlite3*>(dbConnection);
    if (!db) return false;
    
    const char* sql = "UPDATE messages SET is_read = TRUE WHERE id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return false;
    
    sqlite3_bind_int(stmt, 1, messageId);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE && sqlite3_changes(db) > 0;
}

std::vector<Message> Database::getMessageHistory(const std::string& user1, const std::string& user2) {
    std::vector<Message> messages;
    sqlite3* db = static_cast<sqlite3*>(dbConnection);
    if (!db) return messages;
    
    const char* sql = R"(
        SELECT id, sender, receiver, content, timestamp, is_read, is_edited 
        FROM messages 
        WHERE (sender = ? AND receiver = ?) OR (sender = ? AND receiver = ?)
        ORDER BY timestamp ASC
    )";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return messages;
    
    sqlite3_bind_text(stmt, 1, user1.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, user2.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, user2.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, user1.c_str(), -1, SQLITE_STATIC);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Message msg;
        msg.id = sqlite3_column_int(stmt, 0);
        msg.sender = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        msg.receiver = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        msg.content = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        msg.timestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        msg.isRead = sqlite3_column_int(stmt, 5) != 0;
        msg.isEdited = sqlite3_column_int(stmt, 6) != 0;
        
        messages.push_back(msg);
    }
    
    sqlite3_finalize(stmt);
    return messages;
}

std::vector<Message> Database::getChatroomMessages(const std::string& chatroomName) {
    std::vector<Message> messages;
    sqlite3* db = static_cast<sqlite3*>(dbConnection);
    if (!db) return messages;
    
    const char* sql = R"(
        SELECT id, sender, receiver, content, timestamp, is_read, is_edited 
        FROM messages 
        WHERE receiver = ?
        ORDER BY timestamp ASC
    )";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return messages;
    
    sqlite3_bind_text(stmt, 1, chatroomName.c_str(), -1, SQLITE_STATIC);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Message msg;
        msg.id = sqlite3_column_int(stmt, 0);
        msg.sender = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        msg.receiver = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        msg.content = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        msg.timestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        msg.isRead = sqlite3_column_int(stmt, 5) != 0;
        msg.isEdited = sqlite3_column_int(stmt, 6) != 0;
        
        messages.push_back(msg);
    }
    
    sqlite3_finalize(stmt);
    return messages;
}

int Database::getTotalMessagesSent(const std::string& username) {
    sqlite3* db = static_cast<sqlite3*>(dbConnection);
    if (!db) return 0;
    
    const char* sql = "SELECT COUNT(*) FROM messages WHERE sender = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return 0;
    
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return count;
}

int Database::getUnreadMessageCount(const std::string& username) {
    sqlite3* db = static_cast<sqlite3*>(dbConnection);
    if (!db) return 0;
    
    const char* sql = "SELECT COUNT(*) FROM messages WHERE receiver = ? AND is_read = FALSE";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return 0;
    
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return count;
}

std::vector<Chat> Database::getUserChats(const std::string& username) {
    std::vector<Chat> chats;
    sqlite3* db = static_cast<sqlite3*>(dbConnection);
    if (!db) return chats;
    
    // Get direct message chats
    const char* dmSql = R"(
        SELECT 
            CASE 
                WHEN sender = ? THEN receiver 
                ELSE sender 
            END as chat_partner,
            'direct' as type,
            content as last_message,
            timestamp as last_message_time,
            (SELECT COUNT(*) FROM messages m2 
             WHERE ((m2.sender = messages.sender AND m2.receiver = messages.receiver) OR 
                    (m2.sender = messages.receiver AND m2.receiver = messages.sender))
               AND m2.receiver = ? AND m2.is_read = FALSE) as unread_count
        FROM messages 
        WHERE sender = ? OR receiver = ?
        GROUP BY 
            CASE 
                WHEN sender = ? THEN receiver 
                ELSE sender 
            END
        ORDER BY timestamp DESC
    )";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, dmSql, -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, username.c_str(), -1, SQLITE_STATIC);
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Chat chat;
            chat.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            chat.type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            chat.lastMessage = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            chat.lastMessageTime = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            chat.unreadCount = sqlite3_column_int(stmt, 4);
            chat.isActive = true;
            
            chats.push_back(chat);
        }
    }
    sqlite3_finalize(stmt);
    
    // Get chatroom chats
    const char* chatroomSql = R"(
        SELECT 
            cm.chatroom_name as name,
            'chatroom' as type,
            COALESCE(m.content, '') as last_message,
            COALESCE(m.timestamp, '') as last_message_time,
            COALESCE((SELECT COUNT(*) FROM messages m2 
                      WHERE m2.receiver = cm.chatroom_name 
                        AND m2.is_read = FALSE 
                        AND m2.sender != ?), 0) as unread_count
        FROM chatroom_members cm
        LEFT JOIN (
            SELECT receiver, content, timestamp,
                   ROW_NUMBER() OVER (PARTITION BY receiver ORDER BY timestamp DESC) as rn
            FROM messages 
            WHERE receiver IN (SELECT chatroom_name FROM chatroom_members WHERE username = ?)
        ) m ON cm.chatroom_name = m.receiver AND m.rn = 1
        WHERE cm.username = ?
        ORDER BY m.timestamp DESC
    )";
    
    rc = sqlite3_prepare_v2(db, chatroomSql, -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, username.c_str(), -1, SQLITE_STATIC);
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Chat chat;
            chat.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            chat.type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            chat.lastMessage = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            chat.lastMessageTime = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            chat.unreadCount = sqlite3_column_int(stmt, 4);
            chat.isActive = true;
            
            chats.push_back(chat);
        }
    }
    sqlite3_finalize(stmt);
    
    return chats;
}