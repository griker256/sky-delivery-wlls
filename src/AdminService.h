#ifndef ADMIN_SERVICE_H
#define ADMIN_SERVICE_H

#include <sqlite3.h>
#include <vector>
#include "Models.h"

using namespace std;

class AdminService
{
private:
    sqlite3* db;

public:
    AdminService(sqlite3* database) : db(database) {}

    bool registerAdmin(const string& name, const string& phone, const string& password);
    bool adminLogin(const string& password);
    vector<Admin> getAllAdmins();
    bool updateAdminMarkup(int commodityId, double markup);
    bool updateDeliveryFee(int commodityId, double fee);
};

#endif
