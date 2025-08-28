#include "../include/MessageHandler.h"
#include <random>
#include <regex>
#include <unordered_set>
#include <iostream>
#include <codecvt> // اضافه شده
#include <locale>  // اضافه شده

std::vector<Message> MessageManager::messages;

// اضافه کردن توابع تبدیل encoding
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

// اضافه کردن توابع تبدیل پیام
Message MessageManager::your_msg_to_db_msg(const ::Message& yourMsg) {
    Message dbMsg;

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

    // تبدیل time_t به string timestamp
    std::tm* tm = std::localtime(&yourMsg.timestamp);
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);
    dbMsg.timestamp = buffer;

    dbMsg.isRead = yourMsg.is_read;
    dbMsg.isEdited = !yourMsg.is_editable;

    return dbMsg;
}

::Message MessageManager::db_msg_to_your_msg(const Message& dbMsg) {
    ::Message yourMsg;

    yourMsg.id = std::to_wstring(dbMsg.id);
    yourMsg.sender = str_to_wstr(dbMsg.sender);
    yourMsg.receiver = str_to_wstr(dbMsg.receiver);
    yourMsg.content = str_to_wstr(dbMsg.content);

    // تبدیل string timestamp به time_t
    std::tm tm = {};
    std::istringstream ss(dbMsg.timestamp);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    yourMsg.timestamp = std::mktime(&tm);

    yourMsg.is_read = dbMsg.isRead;
    yourMsg.is_editable = !dbMsg.isEdited;

    return yourMsg;
}


const Message* MessageManager::find_message(const std::wstring& id) {
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



void MessageManager::send(const Message& msg, const std::wstring& attachment_path) {
    if (!is_valid_message(msg.content)) {
        return;
    }

    Message new_msg = msg;
    new_msg.content = filterEmojis(new_msg.content);

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

    std::wcout << L"✅ پیام با موفقیت ارسال شد (" << new_msg.content.length() << L" کاراکتر)\n";
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
            return true;
        }
    }
    return false;
}

bool MessageManager::can_edit_message(const Message& msg) {
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
            }

            return true;
        }
    }
    return false;
}

bool MessageManager::is_message_deleted(const Message& msg) {
    return msg.deleted_by_sender || msg.deleted_by_receiver;
}

bool MessageManager::forward_message(const std::wstring& id, const std::wstring& new_receiver) {
    const Message* original = find_message(id);
    if (original) {
        Message forwarded_msg = *original;
        forwarded_msg.id = generate_id();
        forwarded_msg.receiver = new_receiver;
        forwarded_msg.is_forwarded = true;
        forwarded_msg.timestamp = time(nullptr);
        messages.push_back(forwarded_msg);
        return true;
    }
    return false;
}

Message MessageManager::create_reply(const std::wstring& original_id, const std::wstring& sender,
                                     const std::wstring& reply_content) {
    Message reply;
    reply.id = generate_id();
    reply.sender = sender;
    reply.replied_to_id = original_id;
    reply.content = reply_content;
    reply.timestamp = time(nullptr);

    const Message* original = find_message(original_id);
    if (original) {
        reply.receiver = original->sender;
        reply.replied_to_content = original->content;
        reply.replied_to_sender = original->sender;
    }

    return reply;
}

std::wstring MessageManager::get_reply_preview(const Message& msg) {
    if (msg.replied_to_id.empty()) {
        return msg.content;
    }

    return L"پاسخ به \"" + msg.replied_to_content + L"\" از " +
           msg.replied_to_sender + L": " + msg.content;
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

std::vector<Message> MessageManager::get_last_messages(int limit) {
    std::vector<Message> active_messages;
    for (const auto& msg : messages) {
        if (!(msg.deleted_by_sender && msg.deleted_by_receiver)) {
            active_messages.push_back(msg);
        }
    }

    limit = std::min(limit, static_cast<int>(active_messages.size()));
    return {active_messages.end() - limit, active_messages.end()};
}

bool MessageManager::mark_as_delivered(const std::wstring& id) {
    for (auto& msg : messages) {
        if (msg.id == id && !msg.is_deleted) {
            msg.is_delivered = true;
            msg.delivered_time = time(nullptr);
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
            return true;
        }
    }
    return false;
}

std::wstring MessageManager::get_message_status(const Message& msg) {
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

int MessageManager::get_unread_count(const std::wstring& user) {
    return std::count_if(messages.begin(), messages.end(),
                         [&user](const Message& m) {
                             return !m.is_deleted && m.receiver == user && !m.is_read;
                         });
}
