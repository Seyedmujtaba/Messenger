#include "../include/MessageHandler.h"
#include <random>
#include <regex>
#include <unordered_set>

std::vector<Message> MessageManager::messages;

const std::unordered_set<std::wstring> ALLOWED_EMOJIS = {
        L"😊", L"😂", L"❤️", L"👍", L"👎", L"🔥", L"🎉", L"🤔", L"😢", L"😡"
};

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

bool MessageManager::containsEmoji(const std::wstring& text) {
    static const std::wregex emoji_regex(
            L"[\U0001F600-\U0001F64F\U0001F300-\U0001F5FF\U0001F680-\U0001F6FF\u2600-\u26FF\u2700-\u27BF]"
    );
    return std::regex_search(text, emoji_regex);
}

std::wstring MessageManager::filterEmojis(const std::wstring& text) {
    std::wstring filtered;
    for (size_t i = 0; i < text.size(); ) {
        bool isEmoji = false;

        if (i + 3 < text.size()) {
            std::wstring potentialEmoji(text.begin() + i, text.begin() + i + 4);
            if (ALLOWED_EMOJIS.find(potentialEmoji) != ALLOWED_EMOJIS.end()) {
                filtered += potentialEmoji;
                i += 4;
                isEmoji = true;
            }
        }

        if (!isEmoji && i + 1 < text.size()) {
            std::wstring potentialEmoji(text.begin() + i, text.begin() + i + 2);
            if (ALLOWED_EMOJIS.find(potentialEmoji) != ALLOWED_EMOJIS.end()) {
                filtered += potentialEmoji;
                i += 2;
                isEmoji = true;
            }
        }

        if (!isEmoji) {
            filtered += text[i];
            i++;
        }
    }
    return filtered;
}

void MessageManager::send(const Message& msg, const std::wstring& attachment_path) {
    Message new_msg = msg;
    new_msg.content = filterEmojis(new_msg.content);

    if (!attachment_path.empty()) {
        new_msg.has_attachment = true;
        new_msg.file_path = attachment_path;
        // استخراج نام فایل از مسیر
        size_t last_slash = attachment_path.find_last_of(L"/\\");
        new_msg.file_name = (last_slash == std::wstring::npos) ?
                            attachment_path :
                            attachment_path.substr(last_slash + 1);
    }

    if (new_msg.id.empty()) {
        new_msg.id = generate_id();
    }
    messages.push_back(new_msg);
}

bool MessageManager::edit_message(const std::wstring& id, const std::wstring& new_content) {
    for (auto& msg : messages) {
        if (msg.id == id && msg.is_editable && !msg.is_deleted) {
            msg.content = new_content;
            return true;
        }
    }
    return false;
}

bool MessageManager::delete_message(const std::wstring& id) {
    for (auto& msg : messages) {
        if (msg.id == id && !msg.is_deleted) {
            msg.is_deleted = true;
            msg.content = L"این پیام حذف شده است";
            msg.is_editable = false;
            return true;
        }
    }
    return false;
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
    }

    return reply;
}

std::vector<Message> MessageManager::get_last_messages(int limit) {
    std::vector<Message> active_messages;
    for (const auto& msg : messages) {
        if (!msg.is_deleted) {
            active_messages.push_back(msg);
        }
    }

    limit = std::min(limit, static_cast<int>(active_messages.size()));
    return {active_messages.end() - limit, active_messages.end()};
}

int MessageManager::get_unread_count(const std::wstring& user) {
    return std::count_if(messages.begin(), messages.end(),
                         [&user](const Message& m) {
                             return !m.is_deleted && m.receiver == user && !m.is_read;
                         });
}
