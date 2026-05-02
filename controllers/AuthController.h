#pragma once
// ============================================================
// FILE: src/controllers/AuthController.h
// MODULE: 5 - Controller Layer
// Singleton. Handles login, logout, password hashing,
// session management, permission checks, account locking.
// ============================================================

#include "../utilities/CustomString.h"
#include "../entities/Level5Vehicles.h"  // for AdminUser
#include "../exceptions/TransitExceptions.h"
#include <iostream>

class AuthController {
private:
    bool         m_isLoggedIn;
    CustomString m_currentUser;
    int          m_currentAccessLevel;
    int          m_failedAttempts;
    bool         m_isLocked;

    static const int MAX_ATTEMPTS = 3;

    // [OOP: Private constructor = Singleton]
    AuthController()
        : m_isLoggedIn(false),
          m_currentUser(""),
          m_currentAccessLevel(0),
          m_failedAttempts(0),
          m_isLocked(false) {}

    AuthController(const AuthController&) = delete;
    AuthController& operator=(const AuthController&) = delete;

    // Simple hash: append "_hash" (real system uses bcrypt)
    CustomString hashPassword(const CustomString& pwd) const {
        return pwd + CustomString("_hash");
    }

public:
    // [OOP: Singleton - one shared instance]
    static AuthController& getInstance() {
        static AuthController instance;
        return instance;
    }

    // Login with username and password
    // Returns true if successful, throws on failure
    bool login(const CustomString& username, const CustomString& password) {
        if (m_isLocked) {
            throw UnauthorisedAccessException("AuthController::login");
        }

        // Default admin credentials for demo
        bool validAdmin = (username == CustomString("admin") &&
                           password == CustomString("admin"));

        if (validAdmin) {
            m_isLoggedIn       = true;
            m_currentUser      = username;
            m_currentAccessLevel = 3;  // Super admin
            m_failedAttempts   = 0;
            std::cout << "[AuthController] Login successful: "
                      << username << "\n";
            return true;
        }

        // Failed login
        m_failedAttempts++;
        std::cout << "[AuthController] Failed login attempt "
                  << m_failedAttempts << "/" << MAX_ATTEMPTS << "\n";

        if (m_failedAttempts >= MAX_ATTEMPTS) {
            m_isLocked = true;
            std::cout << "[AuthController] Account LOCKED after "
                      << MAX_ATTEMPTS << " failed attempts!\n";
        }
        throw UnauthorisedAccessException("AuthController::login");
    }

    // Logout
    void logout() {
        m_isLoggedIn         = false;
        m_currentUser        = CustomString("");
        m_currentAccessLevel = 0;
        std::cout << "[AuthController] Logged out.\n";
    }

    // Permission check
    bool hasPermission(int requiredLevel) const {
        if (!m_isLoggedIn) return false;
        return m_currentAccessLevel >= requiredLevel;
    }

    void requirePermission(int level) const {
        if (!hasPermission(level)) {
            throw UnauthorisedAccessException("AuthController::requirePermission");
        }
    }

    // Unlock account (admin action)
    void unlockAccount() {
        m_isLocked       = false;
        m_failedAttempts = 0;
        std::cout << "[AuthController] Account unlocked.\n";
    }

    // Getters
    bool         isLoggedIn()       const { return m_isLoggedIn;         }
    bool         isLocked()         const { return m_isLocked;           }
    CustomString getCurrentUser()   const { return m_currentUser;        }
    int          getAccessLevel()   const { return m_currentAccessLevel; }
};
