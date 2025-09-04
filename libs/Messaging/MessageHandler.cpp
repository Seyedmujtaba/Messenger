#include "../include/MessageHandler.h"
#include <random>
#include <regex>
#include <unordered_set>
#include <iostream>
#include <codecvt>
#include <locale>
#include <ctime>

std::vector<ChatMessage> MessageManager::messages;
Database* MessageManager::db = nullptr;

void MessageManager::initialize(Database* database, const std::wstring& username) {
    db = database;
    if (db) {
        auto dbMessages = db->getUserChats(wstr_to_str(username));
        for (const auto& dbMsg : dbMessages) {
            messages.push_back(db_msg_to_your_msg(dbMsg));
        }
    }
}

void MessageManager::print_all_messages() {
    for (const auto& msg : messages) {
        std::wcout << msg.sender << L" -> " << msg.receiver
                   << L": " << msg.content
                   << L" [" << get_message_status(msg) << L"]\n";
    }
}

std::wstring MessageManager::str_to_wstr(const std::string& str) {
    try {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.from_bytes(str);
    } catch (...) {
        return L"";
    }
}

std::string MessageManager::wstr_to_str(const std::wstring& wstr) {
    try {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.to_bytes(wstr);
    } catch (...) {
        return "";
    }
}

ChatMessage MessageManager::your_msg_to_db_msg(const ::ChatMessage& yourMsg) {
    ChatMessage dbMsg;

    if (!yourMsg.id.empty()) {
        try {
            dbMsg.id = std::stoi(yourMsg.id);
        } catch (...) {
            dbMsg.id = 0;
        }
    }

    dbMsg.sender = wstr_to_str(yourMsg.sender);
    dbMsg.receiver = wstr_to_str(yourMsg.receiver);
    dbMsg.content = wstr_to_str(yourMsg.content);

    std::tm* tm = std::localtime(&yourMsg.timestamp);
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);
    dbMsg.timestamp = buffer;

    dbMsg.isRead = yourMsg.is_read;
    dbMsg.isEdited = !yourMsg.is_editable;

    return dbMsg;
}

::ChatMessage MessageManager::db_msg_to_your_msg(const ChatMessage& dbMsg) {
    ::ChatMessage yourMsg;

    yourMsg.id = std::to_wstring(dbMsg.id);
    yourMsg.sender = str_to_wstr(dbMsg.sender);
    yourMsg.receiver = str_to_wstr(dbMsg.receiver);
    yourMsg.content = str_to_wstr(dbMsg.content);

    std::tm tm = {};
    std::istringstream ss(dbMsg.timestamp);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    yourMsg.timestamp = std::mktime(&tm);

    yourMsg.is_read = dbMsg.isRead;
    yourMsg.is_editable = !dbMsg.isEdited;

    return yourMsg;
}

const ChatMessage* MessageManager::find_message(const std::wstring& id) {
    for (const auto& msg : messages) {
        if (msg.id == id && !msg.is_deleted) {
            return &msg;
        }
    }
    return nullptr;
}

std::wstring generate_id() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    const wchar_t* hex_chars = L"0123456789abcdef";
    std::wstring id;
    for (int i = 0; i < 32; ++i) {
        id += hex_chars[dis(gen)];
    }
    return id;
}

bool MessageManager::send(const ChatMessage& msg, const std::wstring& attachment_path) {
    if (!is_valid_message(msg.content)) {
        return false;
    }

    ChatMessage new_msg = msg;
    // new_msg.content = filterEmojis(new_msg.content); // Uncomment if needed

    if (!attachment_path.empty()) {
        new_msg.has_attachment = true;
        new_msg.file_path = attachment_path;
        size_t last_slash = attachment_path.find_last_of(L"/\\");
        new_msg.file_name = (last_slash == std::wstring::npos) ?
                            attachment_path :
                            attachment_path.substr(last_slash + 1);
    }

    if (new_msg.id.empty()) {
        new_msg.id = generate_id();
    }
    messages.push_back(new_msg);

    // Save to database
    if (db) {
        ChatMessage dbMsg = your_msg_to_db_msg(new_msg);
        db->sendMessage(dbMsg.sender, dbMsg.receiver, dbMsg.content,
                        dbMsg.timestamp, dbMsg.isRead, dbMsg.isEdited);
    }

    std::wcout << L"✅ پیام با موفقیت ارسال شد (" << new_msg.content.length() << L" کاراکتر)\n";
    return true;
}

bool MessageManager::edit_message(const std::wstring& id,
                                  const std::wstring& new_content,
                                  const std::wstring& requester_username) {
    for (auto& msg : messages) {
        if (msg.id == id && msg.sender == requester_username) {
            if (!can_edit_message(msg)) {
                return false;
            }

            msg.content = new_content;

            // Update in database
            if (db) {
                try {
                    int messageId = std::stoi(wstr_to_str(id));
                    ChatMessage dbMsg = your_msg_to_db_msg(msg);
                    db->updateMessage(messageId, dbMsg.content);
                } catch (...) {
                    return false;
                }
            }

            return true;
        }
    }
    return false;
}

bool MessageManager::can_edit_message(const ChatMessage& msg) {
    if (msg.is_deleted || !msg.is_editable) {
        return false;
    }

    const time_t now = time(nullptr);
    const time_t one_week = 7 * 24 * 60 * 60;
    return (now - msg.timestamp) <= one_week;
}

bool MessageManager::delete_message(const std::wstring& id, const std::wstring& username) {
    for (auto& msg : messages) {
        if (msg.id == id) {
            if (msg.sender == username) {
                msg.deleted_by_sender = true;
            } else if (msg.receiver == username) {
                msg.deleted_by_receiver = true;
            } else {
                return false;
            }

            if (msg.deleted_by_sender && msg.deleted_by_receiver) {
                msg.content = L"این پیام حذف شده است";
                msg.is_editable = false;

                // Delete from database
                if (db) {
                    try {
                        int messageId = std::stoi(wstr_to_str(id));
                        db->deleteMessage(messageId);
                    } catch (...) {
                        return false;
                    }
                }
            } else {
                // Update in database for soft delete
                if (db) {
                    try {
                        int messageId = std::stoi(wstr_to_str(id));
                        ChatMessage dbMsg = your_msg_to_db_msg(msg);
                        db->updateMessage(messageId, dbMsg.content);
                    } catch (...) {
                        return false;
                    }
                }
            }

            return true;
        }
    }
    return false;
}

bool MessageManager::is_message_deleted(const ChatMessage& msg) {
    return msg.deleted_by_sender || msg.deleted_by_receiver;
}

bool MessageManager::forward_message(const std::wstring& id, const std::wstring& new_receiver) {
    const ChatMessage* original = find_message(id);
    if (original) {
        ChatMessage forwarded_msg = *original;
        forwarded_msg.id = generate_id();
        forwarded_msg.receiver = new_receiver;
        forwarded_msg.is_forwarded = true;
        forwarded_msg.timestamp = time(nullptr);

        // Save to database
        if (db) {
            ChatMessage dbMsg = your_msg_to_db_msg(forwarded_msg);
            db->sendMessage(dbMsg.sender, dbMsg.receiver, dbMsg.content,
                            dbMsg.timestamp, dbMsg.isRead, dbMsg.isEdited);
        }

        messages.push_back(forwarded_msg);
        return true;
    }
    return false;
}

ChatMessage MessageManager::create_reply(const std::wstring& original_id, const std::wstring& sender,
                                         const std::wstring& reply_content) {
    ChatMessage reply;
    reply.id = generate_id();
    reply.sender = sender;
    reply.replied_to_id = original_id;
    reply.content = reply_content;
    reply.timestamp = time(nullptr);

    const ChatMessage* original = find_message(original_id);
    if (original) {
        reply.receiver = original->sender;
        reply.replied_to_content = original->content;
        reply.replied_to_sender = original->sender;
    }

    // Save to database
    if (db) {
        ChatMessage dbMsg = your_msg_to_db_msg(reply);
        db->sendMessage(dbMsg.sender, dbMsg.receiver, dbMsg.content,
                        dbMsg.timestamp, dbMsg.isRead, dbMsg.isEdited);
    }

    return reply;
}

std::wstring MessageManager::get_reply_preview(const ChatMessage& msg) {
    if (msg.replied_to_id.empty()) {
        return msg.content;
    }

    return L"پاسخ به \"" + msg.replied_to_content + L"\" از " +
           msg.replied_to_sender + L": " + msg.content;
}

std::vector<ChatMessage> MessageManager::get_last_messages(int limit) {
    std::vector<ChatMessage> active_messages;
    for (const auto& msg : messages) {
        if (!(msg.deleted_by_sender && msg.deleted_by_receiver)) {
            active_messages.push_back(msg);
        }
    }

    limit = std::min(limit, static_cast<int>(active_messages.size()));
    return {active_messages.end() - limit, active_messages.end()};
}

std::vector<std::wstring> MessageManager::get_unread_senders(const std::wstring& user) {
    std::vector<std::wstring> senders;
    for (const auto& msg : messages) {
        if (!msg.is_deleted && msg.receiver == user && !msg.is_read) {
            senders.push_back(msg.sender);
        }
    }
    return senders;
}

std::vector<std::pair<std::wstring, int>> MessageManager::get_unread_notifications(const std::wstring& user) {
    std::map<std::wstring, int> sender_count;

    for (const auto& msg : messages) {
        if (!msg.is_deleted && msg.receiver == user && !msg.is_read) {
            sender_count[msg.sender]++;
        }
    }

    std::vector<std::pair<std::wstring, int>> notifications;
    for (const auto& [sender, count] : sender_count) {
        notifications.emplace_back(sender, count);
    }

    return notifications;
}

int MessageManager::get_unread_count(const std::wstring& user) {
    return std::count_if(messages.begin(), messages.end(),
                         [&user](const ChatMessage& m) {
                             return !m.is_deleted && m.receiver == user && !m.is_read;
                         });
}

bool MessageManager::mark_as_delivered(const std::wstring& id) {
    for (auto& msg : messages) {
        if (msg.id == id && !msg.is_deleted) {
            msg.is_delivered = true;
            msg.delivered_time = time(nullptr);

            // Update in database
            if (db) {
                try {
                    int messageId = std::stoi(wstr_to_str(id));
                    db->markAsDelivered(messageId);
                } catch (...) {
                    return false;
                }
            }

            return true;
        }
    }
    return false;
}

bool MessageManager::mark_as_seen(const std::wstring& id) {
    for (auto& msg : messages) {
        if (msg.id == id && !msg.is_deleted) {
            msg.is_seen = true;
            msg.seen_time = time(nullptr);

            // Update in database
            if (db) {
                try {
                    int messageId = std::stoi(wstr_to_str(id));
                    db->markAsSeen(messageId);
                } catch (...) {
                    return false;
                }
            }

            return true;
        }
    }
    return false;
}

std::wstring MessageManager::get_message_status(const ChatMessage& msg) {
    if (msg.is_seen) {
        return L"✅ دیده شده";
    } else if (msg.is_delivered) {
        return L"✓✓ تحویل شده";
    } else {
        return L"✓ ارسال شده";
    }
}

bool MessageManager::is_valid_message(const std::wstring& content) {
    if (content.empty()) {
        std::wcout << L"❌ پیام نمی‌تواند خالی باشد\n";
        return false;
    }

    bool has_meaningful_content = false;
    for (wchar_t c : content) {
        if (c != L' ' && c != L'\n' && c != L'\t') {
            has_meaningful_content = true;
            break;
        }
    }

    if (!has_meaningful_content) {
        std::wcout << L"❌ پیام باید محتوای معنادار داشته باشد\n";
        return false;
    }

    if (content.length() > 1000) {
        std::wcout << L"❌ پیام نمی‌تواند بیشتر از 1000 کاراکتر باشد\n";
        return false;
    }

    return true;
}
