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

using namespace std;

void Command::addCategory() {
    AddCategoryView view;
    view.show();

    DBCategory category = view.getCategory();

    category.save();
}

void Command::listCategories() {
    DBResult<DBCategory> result;
    result.retrieveAll();

    CategoryListView view;
    view.addResults(result);
    view.show();
}

DBCategory Command::getCategory(string & categoryCode) {
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
    string categoryCode;

    if (hasParameters()) {
        categoryCode = getParameter(0);
    }

    DBCategory category = getCategory(categoryCode);

    UpdateCategoryView view;
    view.setCategory(category);
    view.show();

    DBCategory updatedCategory = view.getCategory();
    updatedCategory.save();
}

void Command::deleteCategory() {
    string categoryCode;

    if (hasParameters()) {
        categoryCode = getParameter(0);
    }

    DBCategory category = getCategory(categoryCode);

    category.remove();
    category.clear();
}

void Command::importCategories() {
    string jsonFileName = getParameter(0);

    JFileReader jfile = JFileReader(jsonFileName, "DBCategory");

    vector<JRecord> records = jfile.read("categories");

    for (JRecord & record : records) {
        DBCategory category;

        category.set(record);
        category.save();
    }
}

void Command::exportCategories() {
    string jsonFileName = getParameter(0);

    DBResult<DBCategory> results;
    results.retrieveAll();

    vector<JRecord> records;

    for (int i = 0;i < results.size();i++) {
        DBCategory category = results.at(i);

        JRecord r = category.getRecord();
        records.push_back(r);
    }
    
    JFileWriter jFile = JFileWriter(jsonFileName, "DBCategory");
    jFile.write(records, "categories");
}

void Command::clearCategories() {
    DBCategory category;
    category.removeAll();
}
