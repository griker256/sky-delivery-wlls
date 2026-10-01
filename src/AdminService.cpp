#include "AdminService.h"
#include "Utilities.h"
#include <iostream>

using namespace std;

bool AdminService::registerAdmin(const string& name, const string& phone, const string& password)
{
    vector<Admin> admins = getAllAdmins();

    if (admins.size() >= 2)
    {
        cout << "\nMaximum 2 admins allowed.\n";
        return false;
    }

    const char* sql = "INSERT INTO admins(name, phone, password) VALUES(?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, phone.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, password.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE)
    {
        cerr << "Failed to register admin\n";
        return false;
    }

    cout << "\nAdmin registered successfully.\n";
    return true;
}

bool AdminService::adminLogin(const string& password)
{
    const char* sql = "SELECT id, name, password FROM admins;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        string storedPassword = (const char*)sqlite3_column_text(stmt, 2);

        if (storedPassword == password)
        {
            string name = (const char*)sqlite3_column_text(stmt, 1);
            cout << "\nWelcome Admin " << name << "\n";
            sqlite3_finalize(stmt);
            return true;
        }
    }

    sqlite3_finalize(stmt);
    cout << "\nWrong password.\n";
    return false;
}

vector<Admin> AdminService::getAllAdmins()
{
    vector<Admin> admins;
    const char* sql = "SELECT id, name, phone, password FROM admins;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return admins;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        Admin a;
        a.id = sqlite3_column_int(stmt, 0);
        a.name = (const char*)sqlite3_column_text(stmt, 1);
        a.phone = (const char*)sqlite3_column_text(stmt, 2);
        a.password = (const char*)sqlite3_column_text(stmt, 3);
        admins.push_back(a);
    }

    sqlite3_finalize(stmt);
    return admins;
}

bool AdminService::updateAdminMarkup(int commodityId, double markup)
{
    const char* sql = "UPDATE commodities SET adminMarkup = ? WHERE id = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_double(stmt, 1, markup);
    sqlite3_bind_int(stmt, 2, commodityId);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc == SQLITE_DONE)
    {
        cout << "\nMarkup updated successfully.\n";
        return true;
    }

    return false;
}

bool AdminService::updateDeliveryFee(int commodityId, double fee)
{
    const char* sql = "UPDATE commodities SET deliveryFee = ? WHERE id = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_double(stmt, 1, fee);
    sqlite3_bind_int(stmt, 2, commodityId);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc == SQLITE_DONE)
    {
        cout << "\nDelivery fee updated successfully.\n";
        return true;
    }

    return false;
}
