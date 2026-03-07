#pragma once

#include <string>
#include <unordered_map>

#include "db_public_holiday.h"
#include "db_recurring_charge.h"
#include "db_shortcut.h"
#include "db_transaction.h"
#include "db_transaction_report.h"
#include "db_transfer_transaction_record.h"
#include "pfm_error.h"

class CacheMgr {
    public:
        static CacheMgr & getInstance() {
            static CacheMgr instance;
            return instance;
        }

        CacheMgr(const CacheMgr &) = delete;
        CacheMgr & operator=(const CacheMgr &) = delete;
        CacheMgr(CacheMgr &&) = delete;
        CacheMgr & operator=(CacheMgr &&) = delete;

        ~CacheMgr() = default;

        void clearRecurringCharges() {
            clearCache(recurringChargeBySequence);
        }

        void clearTransactions() {
            clearCache(transactionBySequence);
        }

        void clearReports() {
            clearCache(reportBySequence);
        }

        void clearShortcuts() {
            clearCache(shortcutBySequence);
        }

        void clearHolidays() {
            clearCache(holidayBySequence);
        }

        void clearTransferRecords() {
            clearCache(transferBySequence);
        }

        void clearFindCriteria() {
            findCriteria.clear();
        }

        void addRecurringCharge(int sequence, const DBRecurringCharge & charge) {
            putItem(recurringChargeBySequence, sequence, charge);
        }

        void addTransaction(int sequence, const DBTransaction & transaction) {
            putItem(transactionBySequence, sequence, transaction);
        }

        void addReport(int sequence, const DBTransactionReport & report) {
            putItem(reportBySequence, sequence, report);
        }

        void addShortcut(int sequence, const DBShortcut & shortcut) {
            putItem(shortcutBySequence, sequence, shortcut);
        }

        void addHoliday(int sequence, const DBPublicHoliday & holiday) {
            putItem(holidayBySequence, sequence, holiday);
        }

        void addTransfer(int sequence, const DBTransferTransactionRecord & transfer) {
            putItem(transferBySequence, sequence, transfer);
        }

        const DBRecurringCharge & getRecurringCharge(int sequence) const {
            return getItem(recurringChargeBySequence, sequence, "DBRecurringCharge not found in cache.");
        }

        const DBTransaction & getTransaction(int sequence) const {
            return getItem(transactionBySequence, sequence, "DBTransaction not found in cache.");
        }

        const DBTransactionReport & getReport(int sequence) const {
            return getItem(reportBySequence, sequence, "DBTransactionReport not found in cache.");
        }

        const DBShortcut & getShortcut(int sequence) const {
            return getItem(shortcutBySequence, sequence, "DBShortcut not found in cache.");
        }

        const DBPublicHoliday & getHoliday(int sequence) const {
            return getItem(holidayBySequence, sequence, "DBPublicHoliday not found in cache.");
        }

        const DBTransferTransactionRecord & getTransfer(int sequence) const {
            return getItem(transferBySequence, sequence, "DBTransferRecord not found in cache.");
        }

        void setFindCriteria(const std::string & criteria) {
            findCriteria = criteria;
        }

        const std::string & getFindCriteria() const {
            return findCriteria;
        }

    private:
        template <typename T>
        using SequenceCache = std::unordered_map<int, T>;

        CacheMgr() = default;

        template <typename T>
        static void clearCache(SequenceCache<T> & cache) {
            cache.clear();
        }

        template <typename T>
        static void putItem(SequenceCache<T> & cache, int sequence, const T & item) {
            typename SequenceCache<T>::iterator existing = cache.find(sequence);

            if (existing != cache.end()) {
                existing->second = item;
                return;
            }

            cache.insert(typename SequenceCache<T>::value_type(sequence, item));
        }

        template <typename T>
        static const T & getItem(const SequenceCache<T> & cache, int sequence, const char * errorMessage) {
            typename SequenceCache<T>::const_iterator item = cache.find(sequence);

            if (item != cache.end()) {
                return item->second;
            }

            throw pfm_error(errorMessage);
        }

        SequenceCache<DBRecurringCharge> recurringChargeBySequence;
        SequenceCache<DBTransaction> transactionBySequence;
        SequenceCache<DBTransactionReport> reportBySequence;
        SequenceCache<DBShortcut> shortcutBySequence;
        SequenceCache<DBPublicHoliday> holidayBySequence;
        SequenceCache<DBTransferTransactionRecord> transferBySequence;

        std::string findCriteria;
};
