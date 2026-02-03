#include <iostream>
#include <string>
#include <string.h>
#include <vector>

#include "pfm_error.h"
#include "cli_widget.h"
#include "terminal.h"
#include "custom_widgets.h"
#include "db_config.h"

using namespace std;

#ifndef __CONFIG_VIEW
#define __CONFIG_VIEW

class AddConfigView : public CLIView {
    private:
        CLITextField keyField = CLITextField("Key: ");
        CLITextField valueField = CLITextField("Value: ");
        CLITextField descriptionField = CLITextField("Description: ");

    public:
        AddConfigView() : AddConfigView("Add config item") {}

        AddConfigView(const char * title) : CLIView(title) {
            keyField.setLengthLimit(FIELD_STRING_LEN);
            valueField.setLengthLimit(FIELD_STRING_LEN);
            descriptionField.setLengthLimit(FIELD_STRING_LEN);
        }

        void show() override {
            CLIView::show();

            keyField.show();
            valueField.show();
            descriptionField.show();
        }

        DBConfig getConfig() {
            DBConfig config;

            config.key = keyField.getValue();
            config.value = valueField.getValue();
            config.description = descriptionField.getValue();

            return config;
        }
};

class ChooseConfigView : public CLIView {
    private:
        CLITextField keyField = CLITextField("Key: ");

    public:
        ChooseConfigView() : ChooseConfigView("Use config") {}
        
        ChooseConfigView(const char * title) : CLIView(title) {
            keyField.setLengthLimit(FIELD_STRING_LEN);
        }

        void show() override {
            CLIView::show();

            keyField.show();
        }

        string getKey() {
            return keyField.getValue();
        }
};

class ConfigListView : public CLIListView {
    public:
        ConfigListView() : CLIListView() {
            if (Terminal::getWidth() < getMinimumWidth()) {
                throw pfm_error(
                    pfm_error::buildMsg(
                        "Terminal is not wide enough for ConfigListView. Terminal width %u, minimum width %u", 
                        (unsigned int)Terminal::getWidth(), 
                        (unsigned int)getMinimumWidth()));
            }
        }

        inline uint16_t getMinimumWidth() override {
            return (
                17 + 
                25 + 
                55 + 
                3);
        }

        void addResults(DBResult<DBConfig> & result) {
            char szTitle[TITLE_BUFFER_LEN];

            snprintf(szTitle, TITLE_BUFFER_LEN, "Config items (%d)", result.size());
            setTitle(szTitle);

            setColumns({
                CLIListColumn("Key", 17, CLIListColumn::leftAligned),
                CLIListColumn("Value", 25, CLIListColumn::leftAligned),
                CLIListColumn("Description", 55, CLIListColumn::leftAligned),
                CLIListColumn("R/W", 3, CLIListColumn::leftAligned)
            });

            for (int i = 0;i < result.size();i++) {
                DBConfig config = result[i];

                CLIListRow row(getNumColumns());

                row.addCell(config.key);
                row.addCell(config.value);
                row.addCell(config.description);
                row.addCell(config.isReadOnly ? "RO" : "RW");

                addRow(row);
            }
        }
};

class UpdateConfigView : public CLIView {
    private:
        pfm_id_t configId;

        CLITextField keyField;
        CLITextField valueField;
        CLITextField descriptionField;

    public:
        UpdateConfigView() : UpdateConfigView("Update config item") {}

        UpdateConfigView(const char * title) : CLIView(title) {}

        void setConfig(DBConfig & config) {
            char szPrompt[MAX_PROMPT_LENGTH];

            configId = config.id;

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Key ['%s']: ", config.key.c_str());
            keyField.setLabel(szPrompt);
            keyField.setDefaultValue(config.key);
            keyField.setLengthLimit(FIELD_STRING_LEN);

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Value ['%s']: ", config.value.c_str());
            valueField.setLabel(szPrompt);
            valueField.setDefaultValue(config.value);
            valueField.setLengthLimit(FIELD_STRING_LEN);

            snprintf(szPrompt, MAX_PROMPT_LENGTH, "Description ['%s']: ", config.description.c_str());
            descriptionField.setLabel(szPrompt);
            descriptionField.setDefaultValue(config.description);
            descriptionField.setLengthLimit(FIELD_STRING_LEN);
        }

        void show() override {
            CLIView::show();

            keyField.show();
            valueField.show();
            descriptionField.show();
        }

        DBConfig getConfig() {
            DBConfig config;

            config.id = configId;

            config.key = keyField.getValue();
            config.value = valueField.getValue();
            config.description = descriptionField.getValue();

            if (config.key.length() == 0) {
                throw pfm_error("Config item key must have a value");
            }

            return config;
        }
};

#endif
