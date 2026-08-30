#pragma once

#include <string>

#include "csv.h"

class TransactionReconciler {
    private:
        void populateCSVTempTable(const std::string & accountCode, CSV & csv);

    public:
        void reconcileTransactions(const std::string & accountCode, const std::string & bankCSVName, const std::string & csvMappingName);
};
