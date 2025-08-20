//
// Created by M on 7/28/2025.
//
//Seyedmujtaba Tabatabaee
#ifndef BOTMANAGER_H
#define BOTMANAGER_H

#include <string>
#include <vector>
#include <functional>
#include <optional>
#include <chrono>
#include <unordered_map>

/**
 * @brief انواع آیتم‌های منوی ربات
 */
enum class MenuItem : int {
    UnreadCount = 1,
    UserProfile,
    OnlineContacts,
    LastMessages,
    SearchMessages,
    Reminders,
    Help,
    Settings,
    ClearChatHistory,
    Stats
};

/**
 * @brief ساختار پروفایل کاربر
 */
struct UserProfile {
    std::string id;
    std::string displayName;
    std::string phone;
    bool isOnline = false;
    std::string statusText; // مثال: "در دسترس"، "مزاحم نشوید"
};

/**
 * @brief ساختار پیام
 */
struct Message {
    std::string id;
    std::string fromUserId;
    std::string toUserId;
    std::string text;
    std::chrono::system_clock::time_point timestamp{};
    bool isRead = false;
};

/**
 * @brief ساختار مخاطب
 */
struct Contact {
    std::string id;
    std::string displayName;
    bool isOnline = false;
};

/**
 * @brief ساختار آمار کلی گفتگوها/کاربر
 */
struct Stats {
    size_t sentCount = 0;
    size_t receivedCount = 0;
    std::optional<std::string> topContactId; // بیشترین تعامل
};

/**
 * @brief (اختیاری) ساختار Reminder ساده
 */
struct Reminder {
    std::string id;
    std::string title;
    std::chrono::system_clock::time_point dueAt{};
};

/**
 * @brief توابع وابستگی‌ها (Dependency Injection) برای اتصال لایه‌ی داده/ API
 */
struct BotManagerDeps {
    // تعداد پیام‌های خوانده‌نشده
    std::function<int()> fetchUnreadCount;

    // پروفایل کاربر جاری
    std::function<UserProfile()> fetchUserProfile;

    // مخاطبین آنلاین
    std::function<std::vector<Contact>()> fetchOnlineContacts;

    // آخرین پیام‌ها (limit)
    std::function<std::vector<Message>(size_t /*limit*/)> fetchLastMessages;

    // جست‌وجوی پیام‌ها بر اساس متن (query, limit)
    std::function<std::vector<Message>(const std::string& /*query*/, size_t /*limit*/)> searchMessages;

    // یادآورها
    std::function<std::vector<Reminder>()> fetchReminders;

    // تنظیمات
    std::function<bool(bool /*enable*/)> toggleNotifications; // خروجی: وضعیت نهایی
    std::function<void(const std::string& /*newStatus*/)> setStatusText;

    // پاک‌کردن تاریخچه‌ی چت (برای همین ربات)
    std::function<void()> clearChatHistory;

    // آمار
    std::function<Stats()> fetchStats;
};

/**
 * @brief پیکربندی اولیه‌ی ربات
 */
struct BotManagerConfig {
    std::string botDisplayName = "Helper Bot";
    std::vector<std::string> menuOptionsFA = {
        "1) تعداد پیام‌های خوانده‌نشده",
        "2) پروفایل من",
        "3) مخاطبین آنلاین",
        "4) آخرین پیام‌ها",
        "5) جستجو در پیام‌ها",
        "6) یادآورها",
        "7) راهنما (help)",
        "8) تنظیمات",
        "9) پاک کردن تاریخچه چت بات",
        "10) آمار"
    };
    // محدودیت‌های پیش‌فرض
    size_t defaultLastMessagesLimit = 5;
    size_t defaultSearchLimit = 10;
};

/**
 * @brief مدیر ربات: رابط ساده و UI-ناوابسته برای ارائه‌ی منو و پاسخ‌ها
 *
 * توجه: این کلاس فقط متن خروجی تولید می‌کند (UI-agnostic).
 * برای اتصال به UI/CLI کافی‌ست خروجی‌های متنی را چاپ کنید.
 */
class BotManager {
public:
    explicit BotManager(BotManagerDeps deps,
                        BotManagerConfig cfg = {});

    /**
     * @brief متن منو را برمی‌گرداند (برای چاپ در UI)
     */
    std::string buildMenuText() const;

    /**
     * @brief پردازش انتخاب منو بر اساس شماره‌ی آیتم
     * @param choice شماره‌ی انتخاب‌شده (1..N)
     * @param args آرگومان‌های اختیاری (مثلاً query برای جستجو، limit و ...)
     * @return متن پاسخ آماده‌ی نمایش
     */
    std::string handleChoice(int choice,
                             const std::unordered_map<std::string, std::string>& args = {}) const;

    /**
     * @brief توابع شفاف برای هر قابلیت (در صورت نیاز مستقیم صدا بزنید)
     */
    std::string handleUnreadCount() const;
    std::string handleUserProfile() const;
    std::string handleOnlineContacts() const;
    std::string handleLastMessages(size_t limit) const;
    std::string handleSearchMessages(const std::string& query, size_t limit) const;
    std::string handleReminders() const;
    std::string handleHelp() const;
    std::string handleSettings(const std::unordered_map<std::string, std::string>& args) const;
    std::string handleClearChatHistory() const;
    std::string handleStats() const;

    /**
     * @brief دسترسی به پیکربندی و منو
     */
    const BotManagerConfig& config() const { return config_; }

private:
    BotManagerDeps deps_;
    BotManagerConfig config_;

    // کمک‌کننده‌ها
    static std::string formatTimestamp(const std::chrono::system_clock::time_point& tp);
    static std::string truncate(const std::string& s, size_t maxLen);
};

#endif // BOTMANAGER_H
