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

struct Message {
    std::wstring id;
    std::wstring sender;
    std::wstring receiver;
    std::wstring content;
    time_t timestamp;
    bool is_read = false;
    bool is_editable = true;
    bool is_deleted = false;
    std::wstring replied_to_id;
    bool is_forwarded = false;
    // اضافه کردن فیلدهای جدید برای فایل
    std::wstring file_path;   // مسیر فایل
    std::wstring file_name;   // نام نمایشی فایل
    bool has_attachment = false; // آیا پیام ضمیمه دارد؟

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
    static void send(const Message& msg, const std::wstring& attachment_path = L"");
    static bool edit_message(const std::wstring& id, const std::wstring& new_content);
    static bool delete_message(const std::wstring& id);
    static bool forward_message(const std::wstring& id, const std::wstring& new_receiver);
    static Message create_reply(const std::wstring& original_id, const std::wstring& sender,
                                const std::wstring& reply_content);
    static std::vector<Message> get_last_messages(int limit = 10);
    static int get_unread_count(const std::wstring& user);
    static bool containsEmoji(const std::wstring& text);
    static std::wstring filterEmojis(const std::wstring& text);

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
