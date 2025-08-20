#include "UserManager.h"
#include <iostream>
#include <functional>


UserManager::UserManager(Database& db) : database(db), currentUser("") {}

bool UserManager::registerUser(const std::string& username, const std::string& password) {
    std::string hashed = hashPassword(password);
    return database.createAccount(username, hashed);
}


bool UserManager::loginUser(const std::string& username, const std::string& password) {
    std::string hashed = hashPassword(password);
    if (database.login(username, hashed)) {
        currentUser = username;
        return true;
    }
    return false;
}

void UserManager::logoutUser() {
    currentUser.clear();
}

bool UserManager::sendMessageToUser(const std::string& receiver, const std::string& content) {
    if (!isLoggedIn()) return false;
    return database.sendMessage(currentUser, receiver, content);
}

bool UserManager::sendMessageToChatroom(const std::string& chatroomName, const std::string& content) {
    if (!isLoggedIn()) return false;
    return database.sendMessage(currentUser, chatroomName, content);
}


bool UserManager::isLoggedIn() const {
    return !currentUser.empty();
}

std::string UserManager::getCurrentUser() const {
    return currentUser;
}

std::string UserManager::hashPassword(const std::string& password) {
    std::hash<std::string> hasher;
    return std::to_string(hasher(password));
}

bool UserManager::deleteUser(const std::string& username) {
    if (!isLoggedIn() || currentUser != username) {
        std::cout << "خطا: فقط می‌توانی حساب خودتو حذف کنی.\n";
        return false;
    }

    if (database.deleteAccount(username)) {
        std::cout << "حساب کاربری با موفقیت حذف شد.\n";
        logoutUser();
        return true;
    }
    else {
        std::cout << "خطا در حذف حساب کاربری.\n";
        return false;
    }
}
