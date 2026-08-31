#pragma once

#include <string>
#include <vector>

#include "db_temp_csv.h"
#include "csv.h"

class TransactionReconciler {
    private:
        void dropCSVTempTable();

        std::vector<DBRow> reportPart1(const std::string & accountCode);
        std::vector<DBRow> reportPart2(const std::string & accountCode, const StrDate & startDate, const StrDate & endDate);

        void populateCSVTempTable(const std::string & accountCode, CSV & csv);

    public:
        void reconcileTransactions(const std::string & accountCode, const std::string & bankCSVName, const std::string & csvMappingName);
};
