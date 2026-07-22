#pragma once

#include <string>

class System {
    public:
        static void saveKeyToCredentialStore(const std::string & key);
        static std::string getKeyFromCredentialStore();
        static void deleteKeyFromCredentialStore();

        static std::string getKey(const std::string & prompt);

        static void setIsHeadlessLinux(const bool isHeadless);
        static bool isLikelyHeadlessLinux();
};
