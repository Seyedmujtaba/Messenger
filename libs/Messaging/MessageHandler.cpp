#include "../include/message.hpp"
#include <random>

std::vector<Message> MessageManager::messages;

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

void MessageManager::send(const Message& msg) {
    Message new_msg = msg;
    if (new_msg.id.empty()) {
        new_msg.id = generate_id();
    }
    messages.push_back(new_msg);
}

bool MessageManager::edit_message(const std::wstring& id, const std::wstring& new_content) {
    for (auto& msg : messages) {
        if (msg.id == id && msg.is_editable) {
            msg.content = new_content;
            return true;
        }
    }
    return false;
}

std::vector<Message> MessageManager::get_last_messages(int limit) {
    limit = std::min(limit, static_cast<int>(messages.size()));
    return {messages.end() - limit, messages.end()};
}

int MessageManager::get_unread_count(const std::wstring& user) {
    return std::count_if(messages.begin(), messages.end(),
                         [&user](const Message& m) {
                             return m.receiver == user && !m.is_read;
                         });
}
