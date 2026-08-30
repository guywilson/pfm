#pragma once

#include <string>

#include "csv.h"

class TransactionReconciler {
    private:
        uint64_t findSingleQuotePos(std::string & s, int startingPos) const;
        const std::string delimitSingleQuotes(std::string & s) const;

        void populateCSVTempTable(const std::string & accountCode, CSV & csv);

    public:
        void reconcileTransactions(const std::string & accountCode, const std::string & bankCSVName, const std::string & csvMappingName);
};
