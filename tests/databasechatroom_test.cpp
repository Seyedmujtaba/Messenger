#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include "../libs/Database/Database.h"
#include "../libs/ChatRoom/ChatRoomManager.h"

class FullChatRoomTester {
private:
    std::shared_ptr<Database> database;
    ChatRoomManager chatManager;
    int testCount = 0;
    int passedCount = 0;

public:
    FullChatRoomTester() : database(std::make_shared<Database>("full_test.db")), chatManager(database) {}

    void printTestResult(const std::string& testName, bool success, const std::string& message = "") {
        testCount++;
        if (success) passedCount++;
        
        std::cout << (success ? "✅ PASS" : "❌ FAIL") << " - " << testName;
        if (!message.empty()) {
            std::cout << " : " << message;
        }
        std::cout << std::endl;
    }

    void testMessageEditing() {
        std::cout << "\n1. ✏️ MESSAGE EDITING TESTS" << std::endl;
        std::cout << "==========================================" << std::endl;

        ChatRoom* room = nullptr;
        chatManager.createRoom("Edit Test Room", "For editing tests", "", false, 1, room);

        if (room) {
            // ارسال پیام اولیه
            room->sendMessage(1, "Original message content");
            
            // بازیابی پیام برای گرفتن ID
            auto messages = room->getMessages();
            if (!messages.empty()) {
                int messageId = messages.back().id;
                
                // تست ویرایش پیام
                auto editResult = room->editMessage(messageId, 1, "Edited message content");
                printTestResult("Message editing", editResult.success);
                
                // بررسی محتوای ویرایش شده
                messages = room->getMessages();
                bool contentChanged = !messages.empty() && messages.back().content == "Edited message content";
                printTestResult("Message content updated", contentChanged);
            }
        }
    }

    void testMessageForwarding() {
        std::cout << "\n2. 🔄 MESSAGE FORWARDING TESTS" << std::endl;
        std::cout << "==========================================" << std::endl;

        // ایجاد دو اتاق چت
        ChatRoom* sourceRoom = nullptr;
        ChatRoom* targetRoom = nullptr;
        
        chatManager.createRoom("Source Room", "For forwarding", "", false, 1, sourceRoom);
        chatManager.createRoom("Target Room", "Destination", "", false, 1, targetRoom);

        if (sourceRoom && targetRoom) {
            // افزودن کاربر به هر دو اتاق
            sourceRoom->addMember(1);
            targetRoom->addMember(1);
            
            // ارسال پیام در اتاق مبدأ
            sourceRoom->sendMessage(1, "Message to forward");
            
            // بازیابی پیام برای فوروارد
            auto messages = sourceRoom->getMessages();
            if (!messages.empty()) {
                int messageId = messages.back().id;
                
                // تست فوروارد پیام
                auto forwardResult = sourceRoom->forwardMessage(messageId, 1, *targetRoom);
                printTestResult("Message forwarding", forwardResult.success);
                
                // بررسی پیام فوروارد شده
                auto targetMessages = targetRoom->getMessages();
                bool forwardSuccess = !targetMessages.empty() && 
                                    targetMessages.back().content.find("Forwarded") != std::string::npos;
                printTestResult("Forwarded message received", forwardSuccess);
            }
        }
    }

    void testMessageReadStatus() {
        std::cout << "\n3. 👁️ MESSAGE READ STATUS TESTS" << std::endl;
        std::cout << "==========================================" << std::endl;

        ChatRoom* room = nullptr;
        chatManager.createRoom("Read Status Room", "For read tests", "", false, 1, room);

        if (room) {
            // ارسال پیام
            room->sendMessage(1, "Test message for read status");
            
            auto messages = room->getMessages();
            if (!messages.empty()) {
                int messageId = messages.back().id;
                
                // تست علامت‌گذاری به عنوان خوانده شده
                auto readResult = room->markMessageAsRead(messageId, 1);
                printTestResult("Mark message as read", readResult.success);
                
                // بررسی وضعیت خوانده شده
                messages = room->getMessages();
                bool isRead = !messages.empty() && messages.back().isReadBy(1);
                printTestResult("Message read status updated", isRead);
                
                // تست تعداد پیام‌های خوانده نشده
                int unreadCount = room->getUnreadCount(1);
                printTestResult("Unread message count", unreadCount == 0, 
                               std::to_string(unreadCount) + " unread messages");
            }
        }
    }

    void testMessagePinning() {
        std::cout << "\n4. 📌 MESSAGE PINNING TESTS" << std::endl;
        std::cout << "==========================================" << std::endl;

        ChatRoom* room = nullptr;
        chatManager.createRoom("Pin Test Room", "For pinning tests", "", false, 1, room);

        if (room) {
            // ارسال چند پیام
            room->sendMessage(1, "Regular message 1");
            room->sendMessage(1, "Important message to pin");
            room->sendMessage(1, "Regular message 2");
            
            auto messages = room->getMessages();
            if (messages.size() >= 2) {
                int messageId = messages[1].id; // پیام دوم را pin می‌کنیم
                
                // تست pin کردن پیام
                auto pinResult = room->pinMessage(1, messageId);
                printTestResult("Pin message", pinResult.success);
                
                // بررسی pinned messages
                auto pinned = room->getPinnedMessages();
                bool isPinned = !pinned.empty() && pinned[0] == messageId;
                printTestResult("Message pinned correctly", isPinned);
                
                // تست pin کردن مجدد (باید خطا بدهد)
                auto repinResult = room->pinMessage(1, messageId);
                printTestResult("Prevent duplicate pinning", !repinResult.success);
            }
        }
    }

    void testMessageSearch() {
        std::cout << "\n5. 🔍 MESSAGE SEARCH TESTS" << std::endl;
        std::cout << "==========================================" << std::endl;

        ChatRoom* room = nullptr;
        chatManager.createRoom("Search Test Room", "For search tests", "", false, 1, room);

        if (room) {
            // ارسال پیام‌های مختلف
            room->sendMessage(1, "Hello world message");
            room->sendMessage(1, "Important project update");
            room->sendMessage(1, "Meeting reminder for tomorrow");
            room->sendMessage(1, "Another important notification");
            
            // تست جستجو
            std::vector<ChatMessage> results;
            
            // جستجوی کلمه "important"
            auto searchResult1 = room->searchMessages("important", results);
            printTestResult("Search for 'important'", searchResult1.success && results.size() == 2, 
                           "Found " + std::to_string(results.size()) + " results");
            
            // جستجوی کلمه "meeting"
            results.clear();
            auto searchResult2 = room->searchMessages("meeting", results);
            printTestResult("Search for 'meeting'", searchResult2.success && results.size() == 1);
            
            // جستجوی کلمه غیرموجود
            results.clear();
            auto searchResult3 = room->searchMessages("nonexistent", results);
            printTestResult("Search for non-existent word", searchResult3.success && results.empty());
        }
    }

    void testMessageReplies() {
        std::cout << "\n6. ↩️ MESSAGE REPLY TESTS" << std::endl;
        std::cout << "==========================================" << std::endl;

        ChatRoom* room = nullptr;
        chatManager.createRoom("Reply Test Room", "For reply tests", "", false, 1, room);

        if (room) {
            // ارسال پیام اصلی
            room->sendMessage(1, "Original question");
            
            auto messages = room->getMessages();
            if (!messages.empty()) {
                int originalMessageId = messages.back().id;
                
                // ارسال پاسخ
                auto replyResult = room->sendMessage(1, "This is a reply", "", originalMessageId);
                printTestResult("Send reply message", replyResult.success);
                
                // بررسی پیام‌های پاسخ
                auto replyMessages = room->getMessagesWithReplies();
                printTestResult("Retrieve reply messages", !replyMessages.empty(), 
                               std::to_string(replyMessages.size()) + " replies found");
                
                // بررسی اینکه پیام پاسخ است
                messages = room->getMessages();
                bool isReply = messages.size() >= 2 && messages.back().isReply();
                printTestResult("Message is marked as reply", isReply);
            }
        }
    }

    void testAdminFunctions() {
        std::cout << "\n7. ⚙️ ADMIN FUNCTION TESTS" << std::endl;
        std::cout << "==========================================" << std::endl;

        ChatRoom* room = nullptr;
        chatManager.createRoom("Admin Test Room", "For admin tests", "", false, 1, room);

        if (room) {
            // افزودن کاربر دوم
            room->addMember(2);
            
            // تست تبدیل کاربر به ادمین
            auto addAdminResult = room->addAdmin(2, 1);
            printTestResult("Add user as admin", addAdminResult.success);
            
            // بررسی وضعیت ادمین
            bool isAdmin = room->isAdmin(2);
            printTestResult("User is admin", isAdmin);
            
            // تست حذف ادمین
            auto removeAdminResult = room->removeAdmin(2, 1);
            printTestResult("Remove admin status", removeAdminResult.success);
            
            // تست تنظیمات فقط ادمین‌ها می‌توانند پیام بفرستند
            auto adminOnlyResult = room->setOnlyAdminsCanMessage(true, 1);
            printTestResult("Set admin-only messaging", adminOnlyResult.success);
            
            // تست اینکه کاربر عادی نمی‌تواند پیام بفرستد
            auto sendResult = room->sendMessage(2, "Message from non-admin");
            printTestResult("Prevent non-admin messaging", !sendResult.success);
        }
    }

    void runAllTests() {
        std::cout << "🎯 COMPREHENSIVE CHATROOM FEATURE TESTS" << std::endl;
        std::cout << "==========================================" << std::endl;

        try {
            testMessageEditing();
            testMessageForwarding();
            testMessageReadStatus();
            testMessagePinning();
            testMessageSearch();
            testMessageReplies();
            testAdminFunctions();

            std::cout << "\n==========================================" << std::endl;
            std::cout << "📊 FINAL RESULTS: " << passedCount << "/" << testCount << " tests passed" << std::endl;
            std::cout << "🎯 SUCCESS RATE: " << (passedCount * 100 / testCount) << "%" << std::endl;
            std::cout << "==========================================" << std::endl;

        } catch (const std::exception& e) {
            std::cout << "❌ CRITICAL ERROR: " << e.what() << std::endl;
        }
    }
};

int main() {
    FullChatRoomTester tester;
    tester.runAllTests();
    return 0;
}
