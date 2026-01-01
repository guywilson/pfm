#include <string>
#include <string.h>
#include <vector>
#include <stdlib.h>
#include <ctype.h>
#include <fstream>

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
#include "debug_views.h"

using namespace std;

#define DEFAULT_BACKUP_FILE_NAME                    "pfm_backup.sql"
#define LINE_BUFFER_LENGTH                          4096

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
    Logger & log = Logger::getInstance();

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

        try {
            if (command == "SELECT") {
                vector<DBRow> rows;

                db.executeRead(statement, &rows);

                GenericListView view;
                view.addRows(rows);

                view.show();
            }
            else {
                db.executeWrite(statement);
            }
        }
        catch (pfm_error & e) {
            log.error("SQL mode: Failed to execute statement: %s", e.what());
            cout << "Error: " << e.what() << endl << endl;
        }
    }
}

void Command::backup() {
    string filename;

    if (hasParameters()) {
        filename = getParameter(SIMPLE_PARAM_NAME);
    }
    else {
        filename = DEFAULT_BACKUP_FILE_NAME;
    }

    ofstream os;
    os.open(filename);

    DBConfig config;
    config.backup(os);

    DBShortcut shortcut;
    shortcut.backup(os);

    DBPublicHoliday holiday;
    holiday.backup(os);

    DBTransactionReport report;
    report.backup(os);

    DBCategory category;
    category.backup(os);

    DBPayee payee;
    payee.backup(os);

    DBAccount account;
    account.backup(os);

    DBPrimaryAccount primaryAccount;
    primaryAccount.backup(os);

    DBCarriedOver co;
    co.backup(os);

    DBRecurringCharge charge;
    charge.backup(os);

    DBTransaction transaction;
    transaction.backup(os);

    os.close();
}

void Command::restore() {
    string filename;

    if (hasParameters()) {
        filename = getParameter(SIMPLE_PARAM_NAME);
    }
    else {
        filename = DEFAULT_BACKUP_FILE_NAME;
    }

    ifstream is;
    is.open(filename);

    PFM_DB & db = PFM_DB::getInstance();

    char * lineBuffer = (char *)malloc(LINE_BUFFER_LENGTH);

    if (lineBuffer == NULL) {
        throw pfm_fatal("Failed to allocate memory for SQL import buffer");
    }

    int line = 1;

    try {
        db.begin();

        while (!is.eof()) {
            is.getline(lineBuffer, LINE_BUFFER_LENGTH);
            db.executeWrite(lineBuffer);

            line++;
        }

        DBTransaction::linkTransferTransactions();
        
        db.commit();
    }
    catch (exception & e) {
        db.rollback();
        cout << "Error at line " << to_string(line) << " in file " << filename << " : " << e.what() << endl << endl;
    }

    free(lineBuffer);
    is.close();

    /*
    ** Re-initialise the config, if we started from
    ** a new database, we will have the default config
    ** loaded.
    */
    cfgmgr & cfg = cfgmgr::getInstance();
    cfg.initialise();
}
