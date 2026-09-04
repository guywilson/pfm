#include <string>
#include <string.h>
#include <vector>
#include <stdlib.h>
#include <ctype.h>
#include <fstream>

#include <readline/readline.h>
#include <readline/history.h>

#include "expression.h"
#include "command.h"
#include "money.h"
#include "rlcustom.h"
#include "pfm_error.h"
#include "db.h"
#include "db_audit.h"
#include "system.h"
#include "strdate.h"
#include "cfgmgr.h"
#include "cache.h"
#include "jfile.h"
#include "debug_views.h"
#include "web_api.h"
#include "posixthread.h"


#define DEFAULT_BACKUP_FILE_NAME                    "pfm_backup.sql"
#define LINE_BUFFER_LENGTH                          4096

void Command::changePassword() {
    PFM_DB & db = PFM_DB::getInstance();

    db.changePassword();
}

void Command::getDBKey() {
    std::string accessKey = System::getKey("Access password: ");

    if (accessKey.compare(cfg.getValue("access.key")) == 0) {
        std::string dbKey = System::getKey("Database password: ");
        std::cout << "Key: " << dbKey << std::endl;
    }
    else {
        std::cout << "Invalid access password supplied" << std::endl << std::endl;
    }
}

void Command::saveDBKey() {
    if (System::isLikelyHeadlessLinux()) {
        throw pfm_error("Sorry, passwordless login is not supported on headless (server) systems.");
    }
    
    std::string accessKey = System::getKey("Access password: ");

    if (accessKey.compare(cfg.getValue("access.key")) == 0) {
        std::string dbKey = System::getKey("Database password: ");
        System::saveKeyToCredentialStore(dbKey);
    }
    else {
        std::cout << "Invalid access password supplied" << std::endl << std::endl;
    }
}

void Command::startAPIServer() {
    listener.start();
    PosixThread::sleep(1UL);
}

int Command::getLogLevelParameter(std::string & level) {
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
    std::string level = getParameter(SIMPLE_PARAM_NAME);
    log.addLogLevel(getLogLevelParameter(level));
}

void Command::clearLoggingLevel() {
    std::string level = getParameter(SIMPLE_PARAM_NAME);
    log.clearLogLevel(getLogLevelParameter(level));
}

void Command::enterSQLMode() {
    PFM_DB & db = PFM_DB::getInstance();
    cfgmgr & cfg = cfgmgr::getInstance();
    Logger & log = Logger::getInstance();

    std::string key = System::getKey("Access password: ");

    if (key.compare(cfg.getValue("access.key")) != 0) {
        std::cerr << "Access denied!" << std::endl;
        return;
    }

    clear_history();

    bool loop = true;

    while (loop) {
        rl_utils::setLineLength(512);

        std::string statement = readline("sql > ");

        if (statement == ".quit") {
            loop = false;
            continue;
        }

        add_history(statement.c_str());

        std::string command = statement.substr(0, 6);

        int i = 0;
        for (char & c : command) {
            char x = toupper(c);
            command[i++] = x;
        }

        try {
            if (command == "SELECT") {
                std::vector<DBRow> rows;

                db.executeRead(statement, &rows);

                if (rows.size() > 0) {
                    GenericListView view;
                    view.addRows(rows);

                    view.show();
                }
                else {
                    std::cout << "SELECT statement returned 0 rows" << std::endl << std::endl;
                }
            }
            else {
                db.executeWrite(statement);
            }
        }
        catch (pfm_error & e) {
            log.error("SQL mode: Failed to execute statement: %s", e.what());
            std::cout << "Error: " << e.what() << std::endl << std::endl;
        }
    }
}

void Command::enterCalcMode() {
    clear_history();

    bool loop = true;

    while (loop) {
        rl_utils::setLineLength(256);

        std::string calculation = readline("calc > ");

        if (calculation == "q") {
            loop = false;
            continue;
        }

        add_history(calculation.c_str());

        if (calculation.length() > 0) {
            try {
                Expression expression;
                std::string answer = expression.evaluate(calculation);
                Money result(answer);
                std::cout << "\t" << calculation << " = " << result.localeFormattedStringValue() << std::endl << std::endl;
            }
            catch (calc_error & e) {
                std::cout << "Error: " << e.what() << std::endl << std::endl;
            }
        }
    }
}

DBAuditInteraction Command::getAuditInteraction(int sequence) {
    CacheMgr & cacheMgr = CacheMgr::getInstance();

    DBAuditInteraction ai = cacheMgr.getAuditInteraction(sequence);

    return ai;
}

void Command::listAuditInteractionRecords() {
    StrDate dateAfter = StrDate::nullDate;
    StrDate dateBefore;

    if (hasParameters()) {
        dateAfter = getParameter("date>");

        if (dateAfter.isNull()) {
            throw pfm_validation_error("The 'date>' parameter must be specified");
        }

        dateBefore = getParameter("date<");
    }
    else {
        throw pfm_validation_error("The 'date>' parameter must be specified and optionally the 'date<' can be specified");
    }

    DBAuditInteraction ai;
    DBResult<DBAuditInteraction> results = ai.retrieveByDateRange(dateAfter, dateBefore);

    CacheMgr & cache = CacheMgr::getInstance();

    cache.clearAuditInteractionRecords();

    for (size_t i = 0;i < results.size();i++) {
        DBAuditInteraction auditInteraction = results[i];
        cache.addAuditInteraction(auditInteraction.sequence, auditInteraction);
    }

    AuditInteractionListView view;
    view.addResults(results);

    view.show();
}

void Command::showAuditInteraction() {
    std::string sequence = getParameter(SEQUENCE_PARAM_NAME);
    DBAuditInteraction ai = getAuditInteraction(atoi(sequence.c_str()));

    DBAuditInteraction r;
    r.retrieve(ai.id);

    AuditInteractionDetailsView view;

    view.setInteraction(r);
    view.show();
}

void Command::deleteAuditInteractionRecords() {
    StrDate dateBefore;
    dateBefore.addMonths(-3);

    if (hasParameters()) {
        dateBefore = getParameter("date<");
    }

    std::cout << "Deleting audit interaction records before " << dateBefore.shortDate() << std::endl << std::endl;

    DBAuditInteraction ai;
    ai.deleteBeforeDate(dateBefore);
}

void Command::backup() {
    std::string filename;

    if (hasParameters()) {
        filename = getParameter(SIMPLE_PARAM_NAME);
    }
    else {
        filename = DEFAULT_BACKUP_FILE_NAME;
    }

    std::ofstream os;
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
    std::string filename;

    if (hasParameters()) {
        filename = getParameter(SIMPLE_PARAM_NAME);
    }
    else {
        filename = DEFAULT_BACKUP_FILE_NAME;
    }

    std::ifstream is;
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
    catch (std::exception & e) {
        db.rollback();
        std::cout << "Error at line " << std::to_string(line) << " in file " << filename << " : " << e.what() << std::endl << std::endl;
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
