#include <string>
#include <string.h>
#include <vector>

#include "command.h"
#include "pfm_error.h"
#include "system.h"
#include "strdate.h"
#include "cfgmgr.h"
#include "cache.h"
#include "jfile.h"

#include "db_config.h"
#include "config_views.h"


void Command::addConfig() {
    std::string accessKey = System::getKey("Access password: ");

    if (accessKey.compare(cfg.getValue("access.key")) == 0) {
        AddConfigView view;
        view.show();

        DBConfig config = view.getConfig();

        config.save();
    }
    else {
        std::cout << "Invalid access password supplied" << std::endl << std::endl;
    }
}

void Command::listConfigItems() {
    DBConfig config;
    DBResult<DBConfig> result = config.retrieveAllVisible();

    ConfigDetailsListView view;
    view.addResults(result);
    view.show();
}

DBConfig Command::getConfig(std::string & key) {
    if (key.length() == 0) {
        ChooseConfigView view;
        view.show();

        key = view.getKey();
    }

    DBConfig config;
    config.retrieveByKey(key);

    return config;
}

void Command::updateConfig() {
    std::string configKey;

    if (hasParameters()) {
        configKey = getParameter(SIMPLE_PARAM_NAME);
    }

    DBConfig config = getConfig(configKey);

    if (config.isReadOnly) {
        throw pfm_validation_error(
            pfm_error::buildMsg(
                "Selected config item '%s' is read-only and cannot be modified", 
                config.key.c_str()));
    }

    UpdateConfigView view;
    view.setConfig(config);
    view.show();

    DBConfig updatedConfig = view.getConfig();
    updatedConfig.save();
}

void Command::deleteConfig() {
    std::string configKey;

    if (hasParameters()) {
        configKey = getParameter(SIMPLE_PARAM_NAME);
    }

    DBConfig config = getConfig(configKey);

    std::string accessKey = System::getKey("Access password: ");

    if (accessKey.compare(cfg.getValue("access.key")) == 0) {
        if (config.isReadOnly) {
            throw pfm_validation_error(
                pfm_error::buildMsg(
                    "Selected config item '%s' is read-only and cannot be modified", 
                    config.key.c_str()));
        }
        
        config.remove();
        config.clear();
    }
    else {
        std::cout << "Invalid access password supplied" << std::endl << std::endl;
    }
}
