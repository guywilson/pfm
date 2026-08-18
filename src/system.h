#pragma once

#include <string>
#include <chrono>
#include <functional>
#include <mutex>
#include <optional>
#include <unordered_set>

class System {
    public:
        static void saveKeyToCredentialStore(const std::string & key);
        static std::string getKeyFromCredentialStore();
        static void deleteKeyFromCredentialStore();

        static std::string getKey(const std::string & prompt);

        static void setIsHeadlessLinux(const bool isHeadless);
        static bool isLikelyHeadlessLinux();
};

class SessionManager {
    public:
        using Clock = std::chrono::system_clock;
        using TimePoint = Clock::time_point;
        using Now = std::function<TimePoint()>;

        explicit SessionManager(
                        std::chrono::seconds lifetime = std::chrono::hours(1),
                        Now now = [] { 
                            return Clock::now();
                        });

        // Creates the only active session. Any previous session is revoked.
        [[nodiscard]] std::string createSession();

        // A session may authenticate any number of calls until it expires/revokes.
        [[nodiscard]] bool isValid(const std::string & sessionId);
        [[nodiscard]] bool isValid(const std::string_view & sessionId);
        [[nodiscard]] bool isValid(const char * sessionId);


        // Explicitly ends the current session. Returns true only on a match.
        bool revoke(const std::string & sessionId);

    private:
        struct Session {
            std::string     id;
            TimePoint       expiresAt;
        };

        const std::chrono::seconds lifetime_;
        const Now now_;
        std::mutex mutex_;
        std::optional<Session> active_;

        // Prevents reuse even in the astronomically unlikely event of a collision.
        std::unordered_set<std::string> issued_;

        static std::string generateId();
        
        static bool constantTimeEqual(const std::string & a, const std::string & b);
};

