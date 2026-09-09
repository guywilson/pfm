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

#include "db_category.h"
#include "category_views.h"


void Command::addCategory() {
    AddCategoryView view;
    view.show();

    DBCategory category = view.getCategory();

    category.save();
}

void Command::listCategories() {
    DBCategory category;
    
    DBResult<DBCategory> result = category.retrieveOrderedByCode();

    CategoryListView view;
    view.addResults(result);
    view.show();
}

DBCategory Command::getCategory(std::string & categoryCode) {
    if (categoryCode.length() == 0) {
        ChooseCategoryView view;
        view.show();

        categoryCode = view.getCode();
    }

    DBCategory category;
    category.retrieveByCode(categoryCode);

    return category;
}

void Command::updateCategory() {
    std::string categoryCode;

    if (hasParameters()) {
        categoryCode = getParameter(SIMPLE_PARAM_NAME);
    }

    DBCategory category = getCategory(categoryCode);

    UpdateCategoryView view;
    view.setCategory(category);
    view.show();

    DBCategory updatedCategory = view.getCategory();
    updatedCategory.save();
}

void Command::deleteCategory() {
    std::string categoryCode;

    if (hasParameters()) {
        categoryCode = getParameter(SIMPLE_PARAM_NAME);
    }

    DBCategory category = getCategory(categoryCode);

    category.remove();
    category.clear();
}

void Command::importCategories() {
    std::string jsonFileName = getParameter(SIMPLE_PARAM_NAME);

    JFileReader jfile = JFileReader(jsonFileName);
    jfile.validate("DBCategory");

    std::vector<JRecord> records = jfile.read("categories");

    for (JRecord & record : records) {
        DBCategory category;

        category.set(record);
        category.save();
    }
}

void Command::exportCategories() {
    std::string jsonFileName = getParameter(SIMPLE_PARAM_NAME);

    DBResult<DBCategory> results;
    results.retrieveAll();

    std::vector<JRecord> records;

    for (size_t i = 0;i < results.size();i++) {
        DBCategory category = results.at(i);

        JRecord r = category.getRecord();
        records.push_back(r);
    }
    
    JFileWriter jFile(jsonFileName, "DBCategory");
    jFile.write(records, "categories");
}

void Command::clearCategories() {
    DBCategory category;
    category.removeAll();
}
