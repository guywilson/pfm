#include <iostream>
#include <string>
#include <string.h>
#include <vector>

#include <readline/readline.h>
#include <readline/history.h>

#include "pfm_error.h"
#include "cli_widget.h"
#include "terminal.h"
#include "custom_widgets.h"
#include "db_shortcut.h"
#include "cfgmgr.h"

using namespace std;

#ifndef __REPORT_VIEW
#define __REPORT_VIEW

#define CRITERIA_BUFFER_LENGTH                      64
#define SHORTCUT_PROMPT_LENGTH                      80

class AddShortcutView : public CLIView {
    private:
        CLITextField shortcutField = CLITextField("Shortcut: ");
        CLITextField replacementTextField = CLITextField("Replacement text: ");

    public:
        AddShortcutView() : AddShortcutView("Add shortcut") {}

        AddShortcutView(const char * title) : CLIView(title) {
        }

        void show() override {
            CLIView::show();

            shortcutField.show();
            replacementTextField.show();
        }

        DBShortcut getShortcut() {
            DBShortcut shortcut;

            shortcut.shortcut = shortcutField.getValue();
            shortcut.replacementText = replacementTextField.getValue();

            return shortcut;
        }
};

class ShortcutListView : public CLIListView {
    private:
        void buildResultsTable(DBResult<DBShortcut> & result) {
            reserveRows(result.size());
            
            setColumns({
                CLIListColumn("Seq", LIST_VIEW_SEQUENCE_WIDTH, CLIListColumn::rightAligned),
                CLIListColumn("Shortcut", 12, CLIListColumn::leftAligned),
                CLIListColumn("Replacement", 80, CLIListColumn::leftAligned)
            });

            for (size_t i = 0;i < result.size();i++) {
                DBShortcut shortcut = result[i];

                CLIListRow row(getNumColumns());

                row.addCell(shortcut.sequence);
                row.addCell(shortcut.shortcut);
                row.addCell(shortcut.replacementText);

                addRow(row);
            }
        }

    public:
        ShortcutListView() : CLIListView() {
            if (Terminal::getWidth() < getMinimumWidth()) {
                throw pfm_error(
                    pfm_error::buildMsg(
                        "Terminal is not wide enough for ShortcutListView. Terminal width %u, minimum width %u", 
                        (unsigned int)Terminal::getWidth(), 
                        (unsigned int)getMinimumWidth()));
            }
        }

        inline uint16_t getMinimumWidth() override {
            return (
                3 + 
                10 + 
                80);
        }

        void addResults(DBResult<DBShortcut> & result) {
            buildResultsTable(result);

            char szTitle[TITLE_BUFFER_LEN];

            snprintf(szTitle, TITLE_BUFFER_LEN, "Shortcut list (%zu) [%d]", result.size(), getTotalWidth());
            setTitle(szTitle);
        }

        void show() override {
            CLIListView::show();
        }
};

class ShortcutDetailsListView : public CLIDetailListView {
    private:
        std::string formatSequence(uint32_t sequence) {
            char sqStr[8];
            snprintf(sqStr, 8, "%03u", sequence);
            return std::string(sqStr);
        }

    public:
        void addResults(DBResult<DBShortcut> & result) {
            char szTitle[TITLE_BUFFER_LEN];

            snprintf(szTitle, TITLE_BUFFER_LEN, "Shortcut list (%zu)", result.size());
            setTitle(szTitle);

            for (size_t i = 0;i < result.size();i++) {
                DBShortcut shortcut = result[i];

                CLIDetailListRow row;

                row.addPrimaryCell(CLIDetailListCell(formatSequence(shortcut.sequence), 3, TextStyle::NoStyle, Colour::Cyan));
                row.addPrimaryCell(CLIDetailListCell(shortcut.shortcut, 71, TextStyle::NoStyle, Colour::White));

                row.addSecondaryCell(CLIDetailListCell("", 3, TextStyle::NoStyle, Colour::Default));
                row.addSecondaryCell(CLIDetailListCell(shortcut.replacementText, 71, TextStyle::Bold, Colour::Green));

                addRow(row);
            }
        }

        void show() override {
            CLIDetailListView::show();
            cout << endl;
        }
};

class UpdateShortcutView : public CLIView {
    private:
        pfm_id_t shortcutId;

        CLITextField shortcutField;
        CLITextField replacementTextField;

    public:
        UpdateShortcutView() : UpdateShortcutView("Update shortcut") {}

        UpdateShortcutView(const char * title) : CLIView(title) {
            replacementTextField.setLengthLimit(SHORTCUT_PROMPT_LENGTH);
        }

        void setShortcut(DBShortcut & shortcut) {
            char szPrompt[SHORTCUT_PROMPT_LENGTH];

            shortcut.retrieve();

            shortcutId = shortcut.id;

            snprintf(szPrompt, SHORTCUT_PROMPT_LENGTH, "Shortcut ['%s']: ", shortcut.shortcut.c_str());
            shortcutField.setLabel(szPrompt);
            shortcutField.setDefaultValue(shortcut.shortcut);

            snprintf(szPrompt, SHORTCUT_PROMPT_LENGTH, "Replacement ['%s']: ", shortcut.replacementText.c_str());
            replacementTextField.setLabel(szPrompt);
            replacementTextField.setDefaultValue(shortcut.replacementText);
        }

        void show() override {
            CLIView::show();

            shortcutField.show();
            replacementTextField.show();
        }

        DBShortcut getShortcut() {
            DBShortcut shortcut;

            shortcut.id = shortcutId;

            shortcut.shortcut = shortcutField.getValue();
            shortcut.replacementText = replacementTextField.getValue();

            return shortcut;
        }
};

#endif
