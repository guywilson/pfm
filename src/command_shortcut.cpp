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


void Command::addShortcut() {
    AddShortcutView view;
    view.show();

    DBShortcut shortcut = view.getShortcut();
    shortcut.save();

    std::vector<std::pair<std::string, std::string>> pairs = DBShortcut::populate();
    rl_utils::loadShortcuts(pairs);
}

void Command::listShortcuts() {
    DBResult<DBShortcut> result;
    result.retrieveAll();

    CacheMgr & cacheMgr = CacheMgr::getInstance();

    cacheMgr.clearShortcuts();

    for (size_t i = 0;i < result.size();i++) {
        DBShortcut shortcut = result[i];
        cacheMgr.addShortcut(shortcut.sequence, shortcut);
    }

    if (Terminal::isOverWidthThreshold()) {
        ShortcutListView view;
        view.addResults(result);
        view.show();
    }
    else {
        ShortcutDetailsListView view;
        view.addResults(result);
        view.show();
    }
}

DBShortcut Command::getShortcut(int sequence) {
    CacheMgr & cacheMgr = CacheMgr::getInstance();

    DBShortcut shortcut = cacheMgr.getShortcut(sequence);

    return shortcut;
}

void Command::updateShortcut() {
    std::string sequence = getParameter(SEQUENCE_PARAM_NAME);

    DBShortcut shortcut = getShortcut(atoi(sequence.c_str()));

    UpdateShortcutView view;
    view.setShortcut(shortcut);
    view.show();

    DBShortcut updatedShortcut = view.getShortcut();
    updatedShortcut.save();

    std::vector<std::pair<std::string, std::string>> pairs = DBShortcut::populate();
    rl_utils::loadShortcuts(pairs);
}

void Command::deleteShortcut() {
    std::string sequence = getParameter(SEQUENCE_PARAM_NAME);

    DBShortcut shortcut = getShortcut(atoi(sequence.c_str()));

    shortcut.remove();
    shortcut.clear();

    std::vector<std::pair<std::string, std::string>> pairs = DBShortcut::populate();
    rl_utils::loadShortcuts(pairs);
}
