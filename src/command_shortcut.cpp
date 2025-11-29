#include <string>
#include <string.h>
#include <vector>
#include <stdlib.h>

#include "command.h"
#include "pfm_error.h"
#include "db.h"
#include "strdate.h"
#include "cfgmgr.h"
#include "cache.h"
#include "jfile.h"

#include "rlcustom.h"
#include "db_shortcut.h"
#include "shortcut_views.h"

using namespace std;

void Command::addShortcut() {
    AddShortcutView view;
    view.show();

    DBShortcut shortcut = view.getShortcut();
    shortcut.save();

    rl_utils::loadShortcuts();
}

void Command::listShortcuts() {
    DBResult<DBShortcut> result;
    result.retrieveAll();

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    cacheMgr.clearShortcuts();

    for (int i = 0;i < result.size();i++) {
        DBShortcut shortcut = result[i];
        cacheMgr.addShortcut(shortcut.sequence, shortcut);
    }

    ShortcutListView view;
    view.addResults(result);
    view.show();
}

DBShortcut Command::getShortcut(int sequence) {
    CacheMgr & cacheMgr = CacheMgr::getInstance();

    DBShortcut shortcut = cacheMgr.getShortcut(sequence);

    return shortcut;
}

void Command::updateShortcut() {
    string sequence = getParameter(SEQUENCE_PARAM_NAME);

    DBShortcut shortcut = getShortcut(atoi(sequence.c_str()));

    UpdateShortcutView view;
    view.setShortcut(shortcut);
    view.show();

    DBShortcut updatedShortcut = view.getShortcut();
    updatedShortcut.save();
}

void Command::deleteShortcut() {
    string sequence = getParameter(SEQUENCE_PARAM_NAME);

    DBShortcut shortcut = getShortcut(atoi(sequence.c_str()));

    shortcut.remove();
    shortcut.clear();
}
