/*
 * Copyright (C) 2025-2026 Guy Wilson
 *
 * This file is part of PFM.
 *
 * PFM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PFM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PFM. If not, see <https://www.gnu.org/licenses/>.
 */

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


void Command::addHoliday() {
    AddHolidayView view;
    view.show();

    DBPublicHoliday holiday = view.getHoliday();

    holiday.save();

    clearPublicHolidays();
    DBPublicHoliday::populatePublicHolidays();
}

void Command::listHolidays() {
    DBResult<DBPublicHoliday> result;
    result.retrieveAll();

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    cacheMgr.clearHolidays();

    for (size_t i = 0;i < result.size();i++) {
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
    std::string sequence = getParameter(SEQUENCE_PARAM_NAME);

    DBPublicHoliday holiday = getHoliday(atoi(sequence.c_str()));

    UpdateHolidayView view;
    view.setHoliday(holiday);
    view.show();

    DBPublicHoliday updatedHoliday = view.getHoliday();
    updatedHoliday.save();

    clearPublicHolidays();
    DBPublicHoliday::populatePublicHolidays();
}

void Command::deleteHoliday() {
    std::string sequence = getParameter(SEQUENCE_PARAM_NAME);

    DBPublicHoliday holiday = getHoliday(atoi(sequence.c_str()));

    holiday.remove();
    holiday.clear();

    clearPublicHolidays();
    DBPublicHoliday::populatePublicHolidays();
}

void Command::importHolidays() {
    std::string jsonFileName = getParameter(SIMPLE_PARAM_NAME);

    JFileReader jfile = JFileReader(jsonFileName);
    jfile.validate("DBPublicHoliday");

    std::vector<JRecord> records = jfile.read("holidays");

    for (JRecord & record : records) {
        DBPublicHoliday holiday;

        holiday.set(record);
        holiday.save();
    }

    clearPublicHolidays();
    DBPublicHoliday::populatePublicHolidays();
}
