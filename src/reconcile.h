#pragma once

#include <string>

#include "db_temp_csv.h"
#include "csv.h"

class TransactionReconciler {
    private:
        void dropCSVTempTable();

        DBResult<DBTempCSV> reportPart1();
        DBResult<DBTransactionView> reportPart2();
        
        void populateCSVTempTable(const std::string & accountCode, CSV & csv);

    public:
        void reconcileTransactions(const std::string & accountCode, const std::string & bankCSVName, const std::string & csvMappingName);
};
