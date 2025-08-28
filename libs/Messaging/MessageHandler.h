#pragma once
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <io.h>
#include <fcntl.h>
#include <algorithm>
#include <codecvt>
#include <locale>
#include "Database.h" // اضافه شده - بسیار مهم!

struct Message {
    std::wstring id;
    std::wstring sender;
    std::wstring receiver;
    std::wstring content;
    std::wstring original_sender;
    std::wstring replied_to_id;
    std::wstring replied_to_content;
    std::wstring replied_to_sender;
    time_t edit_expiry_time;
    time_t timestamp;
    bool is_read = false;
    bool is_editable = true;
    bool is_deleted = false;
    bool deleted_by_sender = false;
    bool deleted_by_receiver = false;
    bool is_delivered = false;
    bool is_seen = false;
    time_t delivered_time = 0;
    time_t seen_time = 0;
    bool is_forwarded = false;
    std::wstring file_path;
    std::wstring file_name;
    bool has_attachment = false;

    std::wstring get_formatted_time() const {
        std::tm tm;
#ifdef _WIN32
        localtime_s(&tm, &timestamp);
#else
        localtime_r(&timestamp, &tm);
#endif
        std::wstringstream wss;
        wss << std::put_time(&tm, L"%Y-%m-%d %H:%M:%S");
        return wss.str();
    }
};

class MessageManager {
private:
    static std::vector<Message> messages;
    static Database* db; // اضافه شده - اشاره‌گر به دیتابیس

    static bool contains_ignore_case(const std::wstring& str, const std::wstring& keyword) {
        auto it = std::search(
                str.begin(), str.end(),
                keyword.begin(), keyword.end(),
                [](wchar_t ch1, wchar_t ch2) {
                    return std::towlower(ch1) == std::towlower(ch2);
                }
        );
        return it != str.end();
    }

    static const Message* find_message(const std::wstring& id);

public:
    // تابع مقداردهی اولیه دیتابیس - اضافه شده
    static void initialize(Database* database);

    // توابع تبدیل encoding - اضافه شده
    static std::wstring str_to_wstr(const std::string& str);
    static std::string wstr_to_str(const std::wstring& wstr);

    // توابع تبدیل پیام - اضافه شده
    static ::Message db_msg_to_your_msg(const Message& dbMsg);
    static Message your_msg_to_db_msg(const ::Message& yourMsg);

    static void send(const Message& msg, const std::wstring& attachment_path = L"");
    static bool edit_message(const std::wstring& id,
                             const std::wstring& new_content,
                             const std::wstring& requester_username);
    static bool can_edit_message(const Message& msg);
    static bool delete_message(const std::wstring& id);
    static bool delete_message(const std::wstring& id, const std::wstring& username);
    static bool is_message_deleted(const Message& msg);
    static bool mark_as_delivered(const std::wstring& id);
    static bool mark_as_seen(const std::wstring& id);
    static bool is_valid_message(const std::wstring& content);
    static std::wstring get_message_status(const Message& msg);
    static bool forward_message(const std::wstring& id, const std::wstring& new_receiver);
    static Message create_reply(const std::wstring& original_id, const std::wstring& sender,
                                const std::wstring& reply_content);
    static std::wstring get_reply_preview(const Message& msg);
    static std::vector<Message> get_last_messages(int limit = 10);
    static std::vector<std::wstring> get_unread_senders(const std::wstring& user);
    static std::vector<std::pair<std::wstring, int>> get_unread_notifications(const std::wstring& user);
    static int get_unread_count(const std::wstring& user);

    static std::pair<int, std::vector<Message>> search_messages(const std::wstring& keyword) {
        std::vector<Message> results;
        for (const auto& msg : messages) {
            if (!msg.is_deleted &&
                (contains_ignore_case(msg.content, keyword) ||
                 contains_ignore_case(msg.sender, keyword) ||
                 contains_ignore_case(msg.receiver, keyword))) {
                results.push_back(msg);
            }
        }
        return {results.size(), results};
    }
};
