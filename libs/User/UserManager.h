#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <string>
#include "Database.h"

class UserManager {
public:
     UserManager(Database& db);

    bool registerUser(const std::string& username, const std::string& password);
    bool loginUser(const std::string& username, const std::string& password);
    void logoutUser();
    bool deleteUser(const std::string& username); 
    bool isLoggedIn() const;
    std::string getCurrentUser() const;

private:
    Database& database;
    std::string currentUser;

    std::string hashPassword(const std::string& password);
};

#endif 
