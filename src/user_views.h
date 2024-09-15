#include <iostream>
#include <string>
#include <string.h>
#include <vector>

#include "pfm_error.h"
#include "cli_widget.h"
#include "custom_widgets.h"
#include "db_user.h"

using namespace std;

#ifndef __USER_VIEW
#define __USER_VIEW

class AddUserView : public CLIView {
    private:
        CLITextField userNameField = CLITextField("User name: ");

    public:
        AddUserView() : AddUserView("Add user") {}

        AddUserView(const char * title) : CLIView(title) {
            userNameField.setLengthLimit(FIELD_STRING_LEN);
        }

        void show() override {
            CLIView::show();

            userNameField.show();
        }

        DBUser getUser() {
            DBUser user;

            user.userName = userNameField.getValue();

            return user;
        }
};

class LoginView : public CLIView {
    private:
        CLITextField userNameField = CLITextField("User name: ");

    public:
        LoginView() : LoginView("Login") {}
        
        LoginView(const char * title) : CLIView(title) {
            userNameField.setLengthLimit(FIELD_STRING_LEN);
        }

        void show() override {
            CLIView::show();

            userNameField.show();
        }

        string getUserName() {
            return userNameField.getValue();
        }
};

#endif
