#include <iostream>
#include <string>
#include <string.h>
#include <vector>

#include "pfm_error.h"
#include "cli_widget.h"
#include "terminal.h"
#include "custom_widgets.h"
#include "db_public_holiday.h"

using namespace std;

#ifndef __PUBLIC_HOLIDAY_VIEW
#define __PUBLIC_HOLIDAY_VIEW

class AddHolidayView : public CLIView {
    private:
        CLITextField dateField = CLITextField("Date: ");
        CLITextField descriptionField = CLITextField("Description: ");

    public:
        AddHolidayView() : AddHolidayView("Add holiday") {}

        AddHolidayView(const char * title) : CLIView(title) {
            dateField.setLengthLimit(DATE_FIELD_LENGTH);
            descriptionField.setLengthLimit(FIELD_STRING_LEN);
        }

        void show() override {
            CLIView::show();

            dateField.show();
            descriptionField.show();
        }

        DBPublicHoliday getHoliday() {
            DBPublicHoliday holiday;

            holiday.date = dateField.getValue();
            holiday.description = descriptionField.getValue();

            return holiday;
        }
};

class ChooseHolidayView : public CLIView {
    private:
        CLITextField sequenceField = CLITextField("Sequence: ");

    public:
        ChooseHolidayView() : ChooseHolidayView("Use holiday") {}
        ChooseHolidayView(const char * title) : CLIView(title) {}

        void show() override {
            CLIView::show();

            sequenceField.show();
        }

        int getSequence() {
            return sequenceField.getIntegerValue();
        }
};

class HolidayListView : public CLIListView {
    public:
        HolidayListView() : CLIListView() {
            if (Terminal::getWidth() < getMinimumWidth()) {
                throw pfm_error(
                    pfm_error::buildMsg(
                        "Terminal is not wide enough for HolidayListView. Terminal width %u, minimum width %u", 
                        (unsigned int)Terminal::getWidth(), 
                        (unsigned int)getMinimumWidth()));
            }
        }

        inline uint16_t getMinimumWidth() override {
            return (
                3 + 
                DATE_FIELD_LENGTH + 
                40);
        }

        void addResults(DBResult<DBPublicHoliday> & result) {
            char szTitle[TITLE_BUFFER_LEN];

            snprintf(szTitle, TITLE_BUFFER_LEN, "Holidays (%zu)", result.size());
            setTitle(szTitle);

            reserveRows(result.size());
            
            setColumns({
                CLIListColumn("Seq", 3, CLIListColumn::rightAligned),
                CLIListColumn("Date", DATE_FIELD_LENGTH, CLIListColumn::leftAligned),
                CLIListColumn("Description", 40, CLIListColumn::leftAligned)
            });

            for (size_t i = 0;i < result.size();i++) {
                DBPublicHoliday holiday = result[i];

                CLIListRow row(getNumColumns());

                row.addCell(holiday.sequence);
                row.addCell(holiday.date);
                row.addCell(holiday.description);

                addRow(row);
            }
        }
};

class UpdateHolidayView : public CLIView {
    private:
        pfm_id_t holidayId;

        CLITextField dateField;
        CLITextField descriptionField;

    public:
        UpdateHolidayView() : UpdateHolidayView("Update holiday") {}

        UpdateHolidayView(const char * title) : CLIView(title) {}

        void setHoliday(DBPublicHoliday & holiday) {
            char szPrompt[MAX_PROMPT_LENGTH];

            holidayId = holiday.id;

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Date ['%s']: ", holiday.date.shortDate().c_str());
            dateField.setLabel(szPrompt);
            dateField.setDefaultValue(holiday.date.shortDate());
            dateField.setLengthLimit(DATE_FIELD_LENGTH);

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Description ['%s']: ", holiday.description.c_str());
            descriptionField.setLabel(szPrompt);
            descriptionField.setDefaultValue(holiday.description);
            descriptionField.setLengthLimit(FIELD_STRING_LEN);
        }

        void show() override {
            CLIView::show();

            dateField.show();
            descriptionField.show();
        }

        DBPublicHoliday getHoliday() {
            DBPublicHoliday holiday;

            holiday.id = holidayId;

            holiday.date = dateField.getValue();
            holiday.description = descriptionField.getValue();

            return holiday;
        }
};

#endif
