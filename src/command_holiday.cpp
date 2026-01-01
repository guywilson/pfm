#include <string>
#include <string.h>
#include <vector>

#include "command.h"
#include "pfm_error.h"
#include "db.h"
#include "strdate.h"
#include "cfgmgr.h"
#include "cache.h"
#include "jfile.h"

#include "db_public_holiday.h"
#include "public_holiday_views.h"

using namespace std;

void Command::addHoliday() {
    AddHolidayView view;
    view.show();

    DBPublicHoliday holiday = view.getHoliday();

    holiday.save();
}

void Command::listHolidays() {
    DBResult<DBPublicHoliday> result;
    result.retrieveAll();

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    cacheMgr.clearHolidays();

    for (int i = 0;i < result.size();i++) {
        DBPublicHoliday holiday = result[i];
        cacheMgr.addHoliday(holiday.sequence, holiday);
    }

    HolidayListView view;
    view.addResults(result);
    view.show();
}

DBPublicHoliday Command::getHoliday(int sequence) {
    int selectedSequence;

    if (sequence == 0) {
        ChooseHolidayView view;
        view.show();

        selectedSequence = view.getSequence();
    }
    else {
        selectedSequence = sequence;
    }

    CacheMgr & cacheMgr = CacheMgr::getInstance();
    DBPublicHoliday holiday = cacheMgr.getHoliday(selectedSequence);

    holiday.retrieve();

    return holiday;
}

void Command::updateHoliday() {
    string sequence = getParameter(SEQUENCE_PARAM_NAME);

    DBPublicHoliday holiday = getHoliday(atoi(sequence.c_str()));

    UpdateHolidayView view;
    view.setHoliday(holiday);
    view.show();

    DBPublicHoliday updatedHoliday = view.getHoliday();
    updatedHoliday.save();
}

void Command::deleteHoliday() {
    string sequence = getParameter(SEQUENCE_PARAM_NAME);

    DBPublicHoliday holiday = getHoliday(atoi(sequence.c_str()));

    holiday.remove();
    holiday.clear();
}

void Command::importHolidays() {
    string jsonFileName = getParameter(SIMPLE_PARAM_NAME);

    JFileReader jfile = JFileReader(jsonFileName);
    jfile.validate("DBPublicHoliday");

    vector<JRecord> records = jfile.read("holidays");

    for (JRecord & record : records) {
        DBPublicHoliday holiday;

        holiday.set(record);
        holiday.save();
    }
}
