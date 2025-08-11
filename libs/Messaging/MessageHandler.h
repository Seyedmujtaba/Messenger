#pragma once
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <io.h>
#include <fcntl.h>
#include <algorithm> // برای توابع جستجو

struct Message {
    std::wstring id;
    std::wstring sender;
    std::wstring receiver;
    std::wstring content;
    time_t timestamp;
    bool is_read = false;
    bool is_editable = true;

    // نمایش زمان به فرمت میلادی
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

    // تابع کمکی برای جستجوی غیرحساس به حروف
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

public:
    static void send(const Message& msg);
    static bool edit_message(const std::wstring& id, const std::wstring& new_content);
    static std::vector<Message> get_last_messages(int limit = 10);
    static int get_unread_count(const std::wstring& user);

    // تابع جستجوی پیشرفته
    static std::pair<int, std::vector<Message>> search_messages(const std::wstring& keyword) {
        std::vector<Message> results;
        for (const auto& msg : messages) {
            if (contains_ignore_case(msg.content, keyword) ||
                contains_ignore_case(msg.sender, keyword) ||
                contains_ignore_case(msg.receiver, keyword)) {
                results.push_back(msg);
            }
        }
        return {results.size(), results};
    }
};
