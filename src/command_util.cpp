#include <string>
#include <string.h>
#include <vector>
#include <stdlib.h>
#include <ctype.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "command.h"
#include "rlcustom.h"
#include "pfm_error.h"
#include "db.h"
#include "strdate.h"
#include "cfgmgr.h"
#include "cache.h"
#include "jfile.h"

using namespace std;

void Command::changePassword() {
    PFM_DB & db = PFM_DB::getInstance();

    db.changePassword();
}

void Command::getDBKey() {
    PFM_DB & db = PFM_DB::getInstance();

    string accessKey = db.getKey("Access password: ");

    if (accessKey.compare(cfg.getValue("access.key")) == 0) {
        string dbKey = db.getKey("Database password: ");
        cout << "Key: " << dbKey << endl;
    }
    else {
        cout << "Invalid access password supplied" << endl << endl;
    }
}

void Command::saveDBKey() {
    PFM_DB & db = PFM_DB::getInstance();

    string accessKey = db.getKey("Access password: ");

    if (accessKey.compare(cfg.getValue("access.key")) == 0) {
        string dbKey = db.getKey("Database password: ");
        db.saveKeyFile(dbKey);
    }
    else {
        cout << "Invalid access password supplied" << endl << endl;
    }
}

int Command::getLogLevelParameter(string & level) {
    int levelID = 0;

    if (level.compare("entry") == 0) {
        levelID = LOG_LEVEL_ENTRY;
    }
    else if (level.compare("exit") == 0) {
        levelID = LOG_LEVEL_EXIT;
    }
    else if (level.compare("debug") == 0) {
        levelID = LOG_LEVEL_DEBUG;
    }
    else if (level.compare("status") == 0) {
        levelID = LOG_LEVEL_STATUS;
    }
    else if (level.compare("info") == 0) {
        levelID = LOG_LEVEL_INFO;
    }
    else if (level.compare("error") == 0) {
        levelID = LOG_LEVEL_ERROR;
    }
    else if (level.compare("fatal") == 0) {
        levelID = LOG_LEVEL_FATAL;
    }
    else if (level.compare("all") == 0) {
        levelID = LOG_LEVEL_ALL;
    }
    else {
        throw pfm_validation_error(
                    pfm_error::buildMsg(
                        "Invalid logging level identifier '%s'", 
                        level.c_str()), 
                    __FILE__, 
                    __LINE__);
    }

    return levelID;
}

void Command::setLoggingLevel() {
    string level = getParameter(SIMPLE_PARAM_NAME);
    log.addLogLevel(getLogLevelParameter(level));
}

void Command::clearLoggingLevel() {
    string level = getParameter(SIMPLE_PARAM_NAME);
    log.clearLogLevel(getLogLevelParameter(level));
}

void Command::enterSQLMode() {
    PFM_DB & db = PFM_DB::getInstance();
    cfgmgr & cfg = cfgmgr::getInstance();

    string key = db.getKey("Access password: ");

    if (key.compare(cfg.getValue("access.key")) != 0) {
        cerr << "Access denied!" << endl;
        return;
    }

    clear_history();

    bool loop = true;

    while (loop) {
        rl_utils::setLineLength(512);

        string statement = readline("sql > ");

        if (statement == ".quit") {
            loop = false;
            continue;
        }

        add_history(statement.c_str());
        
        string command = statement.substr(0, 6);

        int i = 0;
        for (char & c : command) {
            char x = toupper(c);
            command[i++] = x;
        }

        if (command == "SELECT") {
            vector<DBRow> rows;

            db.executeRead(statement, &rows);

            for (int i = 0;i < rows.size();i++) {
                DBRow row = rows[i];

                if (i == 0) {
                    cout << "| ";
                    for (size_t columnNum = 0;columnNum < row.getNumColumns();columnNum++) {
                        DBColumn c = row.getColumnAt(columnNum);

                        cout << c.getName() << " | ";
                    }
                    cout << endl;
                }

                cout << "| ";
                for (size_t columnNum = 0;columnNum < row.getNumColumns();columnNum++) {
                    DBColumn c = row.getColumnAt(columnNum);

                    cout << c.getValue() << " | ";
                }
                cout << endl;
            }
        }
        else {
            db.executeWrite(statement);
        }
    }
}
