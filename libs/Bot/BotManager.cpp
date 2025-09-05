#include "BotManager.h"
#include <sstream>
#include <iomanip>
#include <ctime>
// --- Seyedmujtaba Tabatabaee ---
// --- build menu text ---
std::string BotManager::buildMenuText() const {
    std::ostringstream oss;
    oss << "Bot Menu\n";
    oss << "----------------------\n";
    oss << "1) Unread messages count\n";
    oss << "2) My profile\n";
    oss << "3) Last messages (args: limit)\n";
    oss << "4) Search messages (args: query, limit)\n";
    oss << "5) Reminders\n";
    oss << "6) Stats\n";
    oss << "7) Clear chat history\n";
    oss << "8) Help\n";
    oss << "Enter a choice (1-8):";
    return oss.str();
}

// --- handle choice ---
BotResponse BotManager::handleChoice(const std::string& userId, int choice,
                                     const std::unordered_map<std::string, std::string>& args) {
    if (!userManager.isUserLoggedIn(userId)) {
        return BotResponse("User not logged in. Please login first.", false, 401);
    }

    switch (choice) {
        case static_cast<int>(MenuItem::UnreadCount):
            return handleUnreadCount(userId);
        case static_cast<int>(MenuItem::UserProfile):
            return handleUserProfile(userId);
        case static_cast<int>(MenuItem::LastMessages): {
            size_t limit = 10;
            if (auto it = args.find("limit"); it != args.end()) {
                try { limit = std::stoul(it->second); } catch (...) {}
            }
            return handleLastMessages(userId, limit);
        }
        case static_cast<int>(MenuItem::SearchMessages): {
            std::string query;
            size_t limit = 10;
            if (auto it = args.find("query"); it != args.end()) query = it->second;
            if (auto it = args.find("limit"); it != args.end()) {
                try { limit = std::stoul(it->second); } catch (...) {}
            }
            return handleSearchMessages(userId, query, limit);
        }
        case static_cast<int>(MenuItem::Reminders):
            return handleReminders(userId);
        case static_cast<int>(MenuItem::Stats):
            return handleStats(userId);
        case static_cast<int>(MenuItem::ClearChatHistory):
            return handleClearChatHistory(userId);
        case static_cast<int>(MenuItem::Help):
            return handleHelp();
        default:
            return BotResponse("Invalid choice. Please enter a valid number.", false, 400);
    }
}

// --- handlers ---

BotResponse BotManager::handleUnreadCount(const std::string& userId) const {
    int count = dbManager.fetchUnreadCount(userId);
    std::ostringstream oss;
    oss << "Unread messages: " << count;
    return BotResponse(oss.str());
}

BotResponse BotManager::handleUserProfile(const std::string& userId) const {
    auto profile = userManager.getUserProfile(userId);
    if (!profile.has_value()) {
        return BotResponse("User profile not found.", false, 404);
    }
    std::ostringstream oss;
    oss << "My profile\n"
        << "Name: " << profile->displayName << "\n"
        << "Phone: " << (!profile->phone.empty() ? profile->phone : "-") << "\n"
        << "Presence: " << (profile->isOnline ? "Online" : "Offline") << "\n"
        << "Status: " << (!profile->statusText.empty() ? profile->statusText : "-");
    return BotResponse(oss.str());
}

BotResponse BotManager::handleLastMessages(const std::string& userId, size_t limit) const {
    auto msgs = dbManager.fetchLastMessages(userId, limit);
    if (msgs.empty()) return BotResponse("No messages to show.");
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
    return BotResponse(oss.str());
}

BotResponse BotManager::handleSearchMessages(const std::string& userId, const std::string& query, size_t limit) const {
    if (query.empty()) return BotResponse("Please provide a query string to search.", false, 422);
    auto results = dbManager.searchMessages(userId, query, limit);
    if (results.empty()) {
        return BotResponse("No results for \"" + query + "\".");
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
    return BotResponse(oss.str());
}

BotResponse BotManager::handleReminders(const std::string& userId) const {
    auto rems = dbManager.fetchReminders(userId);
    if (rems.empty()) return BotResponse("No active reminders.");
    std::ostringstream oss;
    oss << "Reminders (" << rems.size() << "):\n";
    size_t idx = 1;
    for (const auto& r : rems) {
        oss << idx++ << ") " << r.title
            << " - due: " << formatTimestamp(r.dueAt) << "\n";
    }
    return BotResponse(oss.str());
}

BotResponse BotManager::handleStats(const std::string& userId) const {
    Stats s = dbManager.fetchStats(userId);
    std::ostringstream oss;
    oss << "Stats:\n"
        << "- Sent: " << s.sentCount << "\n"
        << "- Received: " << s.receivedCount << "\n";
    if (s.topContactId.has_value()) {
        oss << "- Most interaction with: " << *s.topContactId << "\n";
    }
    return BotResponse(oss.str());
}

BotResponse BotManager::handleClearChatHistory(const std::string& userId) const {
    dbManager.clearChatHistory(userId);
    return BotResponse("Bot chat history cleared.");
}

BotResponse BotManager::handleHelp() const {
    std::ostringstream oss;
    oss << "Help:\n"
        << "1) Unread messages count\n"
        << "2) My profile\n"
        << "3) Last messages (args: limit)\n"
        << "4) Search messages (args: query, limit)\n"
        << "5) Reminders\n"
        << "6) Stats\n"
        << "7) Clear chat history\n"
        << "8) Help";
    return BotResponse(oss.str());
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
