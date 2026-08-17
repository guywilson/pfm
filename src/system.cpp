#include <iostream>
#include <string>
#include <cstdlib>
#include <string_view>
#include <array>
#include <cstdint>
#include <fstream>
#include <stdexcept>

#if defined(_WIN32)
#include <windows.h>
#include <bcrypt.h>
#elif defined(__linux__)
#include <cerrno>
#include <sys/random.h>
#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__)
#include <cstdlib>
#endif

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <gcrypt.h>

#include <libcred.hpp>

#include "logger.h"
#include "pfm_error.h"
#include "system.h"

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#endif

#define CREDENTIAL_SERVICE      "pfm-credential-service"
#define CREDENTIAL_ACCOUNT      "admin@pfm.org"
#define PASSWORD_MAX_LEN        256

static bool __isHeadless = false;

static int __getch(void) {
	int		ch;

#ifndef _WIN32
	struct termios current;
	struct termios original;

	tcgetattr(fileno(stdin), &original); /* grab old terminal i/o settings */
	current = original; /* make new settings same as old settings */
	current.c_lflag &= ~ICANON; /* disable buffered i/o */
	current.c_lflag &= ~ECHO; /* set echo mode */
	tcsetattr(fileno(stdin), TCSANOW, &current); /* use these new terminal i/o settings now */
#endif

#ifdef _WIN32
    ch = _getch();
#else
    ch = getchar();
#endif

#ifndef _WIN32
	tcsetattr(0, TCSANOW, &original);
#endif

    return ch;
}

static std::string getPassword(const std::string & prompt) {
    std::cout << prompt;

    char password[PASSWORD_MAX_LEN];
	int	ch = 0;
	int i = 0;
	
    while (ch != '\n') {
        ch = __getch();

        if (ch == EOF) {
            break;
        }

        if (ch != '\n' && ch != '\r') {
            password[i++] = (char)ch;
        }

        if (i == PASSWORD_MAX_LEN - 1) {
            break;
        }
    }

	password[i] = 0;
	
    std::cout << std::endl;
    fflush(stdout);

    return std::string(password);
}

static std::string getKeyFromPassword(const std::string & password) {
	uint32_t keySize = gcry_md_get_algo_dlen(GCRY_MD_SHA3_256);

	uint8_t * keyBuffer = (uint8_t *)malloc(keySize);
	char * k = (char *)malloc((keySize * 2) + 1);

	gcry_md_hash_buffer(GCRY_MD_SHA3_256, keyBuffer, password.c_str(), password.length());

	char hexBuffer[3];
    int j = 0;
    for (int i = 0;i < (int)keySize;i++) {
        snprintf(hexBuffer, 3, "%02X", keyBuffer[i]);

        k[j++] = hexBuffer[0];
        k[j++] = hexBuffer[1];
    }

    k[j] = 0;

    string key(k);

	free(keyBuffer);
	free(k);
	
	return key;
}

#ifdef __linux__
static bool hasGraphicalDisplay() {
    const char* waylandDisplay = std::getenv("WAYLAND_DISPLAY");
    if (waylandDisplay && *waylandDisplay)
        return true;

    const char* x11Display = std::getenv("DISPLAY");
    if (x11Display && *x11Display)
        return true;

    return false;
}
#endif

void secureRandomBytes(unsigned char* output, std::size_t size) {
#if defined(_WIN32)
    if (BCryptGenRandom(nullptr, output, static_cast<ULONG>(size),
                        BCRYPT_USE_SYSTEM_PREFERRED_RNG) != 0) {
        throw std::runtime_error("BCryptGenRandom failed");
    }
#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__)
    arc4random_buf(output, size);
#elif defined(__linux__)
    std::size_t offset = 0;
    while (offset < size) {
        const auto count = ::getrandom(output + offset, size - offset, 0);
        if (count > 0) {
            offset += static_cast<std::size_t>(count);
        } else if (count < 0 && errno == EINTR) {
            continue;
        } else {
            throw std::runtime_error("getrandom failed");
        }
    }
#else
    std::ifstream source("/dev/urandom", std::ios::binary);
    if (!source.read(reinterpret_cast<char*>(output),
                     static_cast<std::streamsize>(size))) {
        throw std::runtime_error("could not read secure random bytes");
    }
#endif
}

std::string base64UrlEncode(const unsigned char* data, std::size_t size) {
    static constexpr char alphabet[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
    std::string result;
    result.reserve((size * 8 + 5) / 6);

    std::uint32_t buffer = 0;
    int bits = 0;
    for (std::size_t i = 0; i < size; ++i) {
        buffer = (buffer << 8) | data[i];
        bits += 8;
        while (bits >= 6) {
            bits -= 6;
            result.push_back(alphabet[(buffer >> bits) & 0x3f]);
        }
    }
    if (bits > 0) {
        result.push_back(alphabet[(buffer << (6 - bits)) & 0x3f]);
    }
    return result;
}

std::string System::getKey(const std::string & prompt) {
    std::string password = getPassword(prompt);
    std::string key = getKeyFromPassword(password);
	
	return key;
}

std::string System::getKeyFromCredentialStore() {
    std::string key;
    std::string lcErrorString;
    libcred::LIBCRED_RESULT result = libcred::get_password(CREDENTIAL_SERVICE, CREDENTIAL_ACCOUNT, &key, &lcErrorString);

    if (result != libcred::SUCCESS) {
        throw pfm_error(pfm_error::buildMsg("Failed to retrieve key from credential store: %s", lcErrorString.c_str()), __FILE__, __LINE__);
    }

    return key;
}

void System::saveKeyToCredentialStore(const std::string & key) {
    std::string lcErrorString;
    libcred::LIBCRED_RESULT result = libcred::set_password(CREDENTIAL_SERVICE, CREDENTIAL_ACCOUNT, key, &lcErrorString);

    if (result != libcred::SUCCESS) {
        throw pfm_error(pfm_error::buildMsg("Failed to save key to credential store: %s", lcErrorString.c_str()), __FILE__, __LINE__);
    }
}

void System::deleteKeyFromCredentialStore() {
    Logger & log = Logger::getInstance();
    
    std::string lcErrorString;
    libcred::LIBCRED_RESULT result = libcred::delete_password(CREDENTIAL_SERVICE, CREDENTIAL_ACCOUNT, &lcErrorString);

    if (result != libcred::SUCCESS) {
        log.error("System::deleteKeyFromCredentialStore() - Failed to delete key from credential store '%s'", lcErrorString.c_str());
    }
    else {
        log.debug("System::deleteKeyFromCredentialStore() - Successfully deleted key from credential store");
    }
}

void System::setIsHeadlessLinux(const bool isHeadless) {
    __isHeadless = isHeadless;
}

bool System::isLikelyHeadlessLinux() {
    if (__isHeadless) {
        return true;
    }
#ifdef __linux__
    return !hasGraphicalDisplay();
#else
    return false;
#endif
}

SessionManager::SessionManager(std::chrono::seconds lifetime, Now now) : lifetime_(lifetime), now_(std::move(now)) {
    if (lifetime_ <= std::chrono::seconds::zero()) {
        throw std::invalid_argument("session lifetime must be positive");
    }
    if (!now_) {
        throw std::invalid_argument("clock function must be provided");
    }
}

std::string SessionManager::createSession() {
    std::lock_guard<std::mutex> lock(mutex_);

    std::string id;
    
    do {
        id = generateId();
    }
    while (issued_.find(id) != issued_.end());

    issued_.insert(id);
    
    active_ = Session{id, now_() + lifetime_};
    
    return id;
}

bool SessionManager::isValid(const std::string& sessionId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!active_) {
        return false;
    }
    
    if (now_() >= active_->expiresAt) {
        active_.reset();
        return false;
    }
    
    return constantTimeEqual(active_->id, sessionId);
}

bool SessionManager::revoke(const std::string& sessionId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!active_ || !constantTimeEqual(active_->id, sessionId)) {
        return false;
    }

    active_.reset();
    
    return true;
}

std::string SessionManager::generateId() {
    std::array<unsigned char, 32> bytes{};
    secureRandomBytes(bytes.data(), bytes.size());
    return base64UrlEncode(bytes.data(), bytes.size());
}

bool SessionManager::constantTimeEqual(const std::string& a,
                                       const std::string& b) {
    std::size_t difference = a.size() ^ b.size();
    const std::size_t maxSize = a.size() > b.size() ? a.size() : b.size();
    for (std::size_t i = 0; i < maxSize; ++i) {
        const unsigned char left =
            i < a.size() ? static_cast<unsigned char>(a[i]) : 0;
        const unsigned char right =
            i < b.size() ? static_cast<unsigned char>(b[i]) : 0;
        difference |= left ^ right;
    }
    return difference == 0;
}
