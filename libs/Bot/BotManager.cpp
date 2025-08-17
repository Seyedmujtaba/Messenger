//
// Created by M on 7/28/2025.
//
//Seyedmujtaba Tabatabaee
#include "BotManager.h"

#include <sstream>
#include <iomanip>
#include <ctime>
#include <utility> // for std::move

// --- constructor ---
BotManager::BotManager(BotManagerDeps deps, BotManagerConfig cfg)
    : deps_(std::move(deps)), config_(std::move(cfg)) {}

// --- build menu text ---
std::string BotManager::buildMenuText() const {
    std::ostringstream oss;
    oss << config_.botDisplayName << "\n";
    oss << "----------------------\n";
    for (const auto& item : config_.menuOptions) {
        oss << item << "\n";
    }
    oss << "Enter a choice (1-" << config_.menuOptions.size() << "):";
    return oss.str();
}

// --- route by choice ---
std::string BotManager::handleChoice(
    int choice,
    const std::unordered_map<std::string, std::string>& args) const
{
    switch (choice) {
        case static_cast<int>(MenuItem::UnreadCount):
            return handleUnreadCount();
        case static_cast<int>(MenuItem::UserProfile):
            return handleUserProfile();
        case static_cast<int>(MenuItem::OnlineContacts):
            return handleOnlineContacts();
        case static_cast<int>(MenuItem::LastMessages): {
            size_t limit = config_.defaultLastMessagesLimit;
            auto it = args.find("limit");
            if (it != args.end()) {
                try { limit = std::stoul(it->second); } catch (...) {}
            }
            return handleLastMessages(limit);
        }
        case static_cast<int>(MenuItem::SearchMessages): {
            std::string query;
            size_t limit = config_.defaultSearchLimit;
            if (auto it = args.find("query"); it != args.end()) query = it->second;
            if (auto it = args.find("limit"); it != args.end()) {
                try { limit = std::stoul(it->second); } catch (...) {}
            }
            return handleSearchMessages(query, limit);
        }
        case static_cast<int>(MenuItem::Reminders):
            return handleReminders();
        case static_cast<int>(MenuItem::Help):
            return handleHelp();
        case static_cast<int>(MenuItem::Settings):
            return handleSettings(args);
        case static_cast<int>(MenuItem::ClearChatHistory):
            return handleClearChatHistory();
        case static_cast<int>(MenuItem::Stats):
            return handleStats();
        default:
            return "Invalid choice. Please enter a valid number.";
    }
}

// --- handlers ---

std::string BotManager::handleUnreadCount() const {
    if (!deps_.fetchUnreadCount) return "Unread count feature is not available.";
    int count = deps_.fetchUnreadCount();
    std::ostringstream oss;
    oss << "Unread messages: " << count;
    return oss.str();
}

std::string BotManager::handleUserProfile() const {
    if (!deps_.fetchUserProfile) return "User profile is not available.";
    UserProfile p = deps_.fetchUserProfile();
    std::ostringstream oss;
    oss << "My profile\n"
        << "Name: " << p.displayName << "\n"
        << "Phone: " << (!p.phone.empty() ? p.phone : "-") << "\n"
        << "Presence: " << (p.isOnline ? "Online" : "Offline") << "\n"
        << "Status: " << (!p.statusText.empty() ? p.statusText : "-");
    return oss.str();
}

std::string BotManager::handleOnlineContacts() const {
    if (!deps_.fetchOnlineContacts) return "Online contacts are not available.";
    auto contacts = deps_.fetchOnlineContacts();
    if (contacts.empty()) return "No online contacts.";
    std::ostringstream oss;
    oss << "Online contacts (" << contacts.size() << "):\n";
    size_t idx = 1;
    for (const auto& c : contacts) {
        oss << idx++ << ") " << c.displayName;
        if (c.isOnline) oss << " - online";
        oss << "\n";
    }
    return oss.str();
}

std::string BotManager::handleLastMessages(size_t limit) const {
    if (!deps_.fetchLastMessages) return "Fetching last messages is not available.";
    auto msgs = deps_.fetchLastMessages(limit);
    if (msgs.empty()) return "No messages to show.";
    std::ostringstream oss;
    oss << "Last messages (" << msgs.size() << "):\n";
    size_t idx = 1;
    for (const auto& m : msgs) {
        oss << idx++ << ") "
            << "[" << formatTimestamp(m.timestamp) << "] "
            << m.fromUserId << " -> " << m.toUserId << " : "
            << truncate(m.text, 80)
            << (m.isRead ? "" : "  (new)") << "\n";
    }
    return oss.str();
}

std::string BotManager::handleSearchMessages(const std::string& query, size_t limit) const {
    if (query.empty()) return "Please provide a query string to search.";
    if (!deps_.searchMessages) return "Search feature is not available.";
    auto results = deps_.searchMessages(query, limit);
    if (results.empty()) {
        return std::string("No results for \"") + query + "\".";
    }
    std::ostringstream oss;
    oss << "Search results for \"" << query << "\" (" << results.size() << "):\n";
    size_t idx = 1;
    for (const auto& m : results) {
        oss << idx++ << ") "
            << "[" << formatTimestamp(m.timestamp) << "] "
            << m.fromUserId << " -> " << m.toUserId << " : "
            << truncate(m.text, 80) << "\n";
    }
    return oss.str();
}

std::string BotManager::handleReminders() const {
    if (!deps_.fetchReminders) return "Reminders are not available.";
    auto rems = deps_.fetchReminders();
    if (rems.empty()) return "No active reminders.";
    std::ostringstream oss;
    oss << "Reminders (" << rems.size() << "):\n";
    size_t idx = 1;
    for (const auto& r : rems) {
        oss << idx++ << ") " << r.title
            << " - due: " << formatTimestamp(r.dueAt) << "\n";
    }
    return oss.str();
}

std::string BotManager::handleHelp() const {
    std::ostringstream oss;
    oss << "Help:\n"
        << "1) Unread messages count\n"
        << "2) My profile\n"
        << "3) Online contacts\n"
        << "4) Last messages (args: limit)\n"
        << "5) Search messages (args: query, limit)\n"
        << "6) Reminders\n"
        << "7) Help\n"
        << "8) Settings (args: notifications=on|off, status=\"...\" )\n"
        << "9) Clear bot chat history\n"
        << "10) Stats";
    return oss.str();
}

std::string BotManager::handleSettings(const std::unordered_map<std::string, std::string>& args) const {
    std::ostringstream oss;
    bool any = false;

    // notifications=on|off
    auto itNotif = args.find("notifications");
    if (itNotif != args.end()) {
        if (!deps_.toggleNotifications) {
            oss << "- Notifications toggle is not available.\n";
        } else {
            bool enable = (itNotif->second == "on" || itNotif->second == "true" || itNotif->second == "1");
            bool finalState = deps_.toggleNotifications(enable);
            oss << "- Notifications: " << (finalState ? "on" : "off") << "\n";
        }
        any = true;
    }

    // status="..."
    auto itStatus = args.find("status");
    if (itStatus != args.end()) {
        if (!deps_.setStatusText) {
            oss << "- Status change is not available.\n";
        } else {
            deps_.setStatusText(itStatus->second);
            oss << "- Status set to: " << itStatus->second << "\n";
        }
        any = true;
    }

    if (!any) {
        oss << "No setting specified. Examples:\n"
            << "notifications=on  or  status=\"Available\"";
    }
    return oss.str();
}

std::string BotManager::handleClearChatHistory() const {
    if (!deps_.clearChatHistory) return "Clear history is not available.";
    deps_.clearChatHistory();
    return "Bot chat history cleared.";
}

std::string BotManager::handleStats() const {
    if (!deps_.fetchStats) return "Stats are not available.";
    Stats s = deps_.fetchStats();
    std::ostringstream oss;
    oss << "Stats:\n"
        << "- Sent: " << s.sentCount << "\n"
        << "- Received: " << s.receivedCount << "\n";
    if (s.topContactId.has_value()) {
        oss << "- Most interaction with: " << *s.topContactId << "\n";
    }
    return oss.str();
}

// --- helpers ---

std::string BotManager::formatTimestamp(const std::chrono::system_clock::time_point& tp) {
    std::time_t tt = std::chrono::system_clock::to_time_t(tp);
    std::tm tm{};
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&tm, &tt);
#else
    localtime_r(&tt, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M");
    return oss.str();
}

std::string BotManager::truncate(const std::string& s, size_t maxLen) {
    if (s.size() <= maxLen) return s;
    if (maxLen <= 3) return "...";
    return s.substr(0, maxLen - 3) + "...";
}
