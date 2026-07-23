#include <iostream>
#include <string>
#include <cstdlib>
#include <string_view>

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
            putchar('*');
            fflush(stdout);

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

static bool hasGraphicalDisplay() {
#ifdef __linux__
    const char* waylandDisplay = std::getenv("WAYLAND_DISPLAY");
    if (waylandDisplay && *waylandDisplay)
        return true;

    const char* x11Display = std::getenv("DISPLAY");
    if (x11Display && *x11Display)
        return true;

    return false;
#else
    // This function only classifies Linux environments.
    return true;
#endif
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