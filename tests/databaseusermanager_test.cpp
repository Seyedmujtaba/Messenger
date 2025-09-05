#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include "../libs/User/UserManager.h"
#include "../libs/Database/Database.h"

// Function to display test results
void printTestResult(const std::string& testName, bool passed) {
    std::cout << testName << ": " << (passed ? "✅ PASSED" : "❌ FAILED") << std::endl;
}

// Main test function
int main() {
    std::cout << "=== 🚀 Starting Comprehensive UserManager and Database Test ===" << std::endl;

    bool allTestsPassed = true;

    try {
        // Test 1: Database Creation
        std::cout << "\n--- Test 1: Create In-Memory Database ---" << std::endl;
        Database db(":memory:");
        std::cout << "Database created successfully" << std::endl;

        // Test 2: UserManager Creation
        std::cout << "\n--- Test 2: Create UserManager ---" << std::endl;
        UserManager userManager(db);
        std::cout << "UserManager created successfully" << std::endl;

        // Test 3: User Registration
        std::cout << "\n--- Test 3: User Registration ---" << std::endl;
        bool test1 = userManager.registerUser("john_doe", "securePassword123");
        printTestResult("Register john_doe", test1);
        allTestsPassed &= test1;

        bool test2 = userManager.registerUser("jane_smith", "strongPass456");
        printTestResult("Register jane_smith", test2);
        allTestsPassed &= test2;

        // Test 4: Duplicate Registration (should fail)
        bool test3 = !userManager.registerUser("john_doe", "differentPassword");
        printTestResult("Duplicate registration (should fail)", test3);
        allTestsPassed &= test3;

        // Test 5: User Login
        std::cout << "\n--- Test 4: User Login ---" << std::endl;
        bool test4 = userManager.loginUser("john_doe", "securePassword123");
        printTestResult("Login john_doe with correct password", test4);
        allTestsPassed &= test4;

        bool test5 = !userManager.loginUser("john_doe", "wrongPassword");
        printTestResult("Login john_doe with wrong password (should fail)", test5);
        allTestsPassed &= test5;

        // Test 6: Login Status
        std::cout << "\n--- Test 5: Login Status ---" << std::endl;
        bool test6 = userManager.isLoggedIn();
        printTestResult("Check if user is logged in", test6);
        allTestsPassed &= test6;

        bool test7 = (userManager.getCurrentUser() == "john_doe");
        printTestResult("Get current username", test7);
        allTestsPassed &= test7;

        // Test 7: Message Sending
        std::cout << "\n--- Test 6: Message Sending ---" << std::endl;
        bool test8 = userManager.sendMessageToUser("jane_smith", "Hello Jane! This is John.");
        printTestResult("Send message to jane_smith", test8);
        allTestsPassed &= test8;

        bool test9 = userManager.sendMessageToChatroom("general", "Hello everyone in the chatroom!");
        printTestResult("Send message to chatroom", test9);
        allTestsPassed &= test9;

        // Test 8: Message History Retrieval
        std::cout << "\n--- Test 7: Message History ---" << std::endl;
        std::vector<Message> history = db.getMessageHistory("john_doe", "jane_smith");
        bool test10 = !history.empty();
        printTestResult("Retrieve message history", test10);
        allTestsPassed &= test10;

        if (test10) {
            std::cout << "Number of messages retrieved: " << history.size() << std::endl;
            std::cout << "Last message content: " << history.back().content << std::endl;
            std::cout << "Sender: " << history.back().sender << std::endl;
            std::cout << "Receiver: " << history.back().receiver << std::endl;
        }

        // Test 9: Message Statistics
        std::cout << "\n--- Test 8: Message Statistics ---" << std::endl;
        int totalMessages = db.getTotalMessagesSent("john_doe");
        std::cout << "Total messages sent by john_doe: " << totalMessages << std::endl;

        int unreadCount = db.getUnreadMessageCount("jane_smith");
        std::cout << "Unread messages for jane_smith: " << unreadCount << std::endl;

        // Test 10: User Chats Overview
        std::cout << "\n--- Test 9: User Chats Overview ---" << std::endl;
        std::vector<Chat> userChats = db.getUserChats("john_doe");
        std::cout << "Number of chat conversations: " << userChats.size() << std::endl;

        // Test 11: Logout
        std::cout << "\n--- Test 10: User Logout ---" << std::endl;
        userManager.logoutUser();
        bool test11 = !userManager.isLoggedIn();
        printTestResult("Check logout status", test11);
        allTestsPassed &= test11;

        // Test 12: Send Message After Logout (should fail)
        bool test12 = !userManager.sendMessageToUser("jane_smith", "This should not be sent");
        printTestResult("Send message after logout (should fail)", test12);
        allTestsPassed &= test12;

        // Test 13: Login Second User
        std::cout << "\n--- Test 11: Second User Login ---" << std::endl;
        bool test13 = userManager.loginUser("jane_smith", "strongPass456");
        printTestResult("Login jane_smith", test13);
        allTestsPassed &= test13;

        if (test13) {
            std::cout << "Current user: " << userManager.getCurrentUser() << std::endl;
        }

    } catch (const std::exception& e) {
        std::cout << "❌ EXCEPTION: " << e.what() << std::endl;
        allTestsPassed = false;
    }

    // Test Summary
    std::cout << "\n=== 📊 Test Summary ===" << std::endl;
    if (allTestsPassed) {
        std::cout << "🎉 ALL TESTS PASSED! System is working correctly." << std::endl;
        std::cout << "✅ UserManager and Database integration is successful" << std::endl;
        std::cout << "✅ All core functionalities are operational" << std::endl;
    } else {
        std::cout << "❌ SOME TESTS FAILED! Review the implementation." << std::endl;
        std::cout << "⚠️  Check for integration issues between modules" << std::endl;
    }

    std::cout << "\n=== Test Completed ===" << std::endl;
    return allTestsPassed ? 0 : 1;
}
