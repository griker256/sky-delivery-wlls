#include "PaymentService.h"
#include "Utilities.h"
#include <iostream>

using namespace std;

bool PaymentService::processPayment(int orderId, const string& paymentMethod)
{
    const char* sql = "UPDATE orders SET paymentStatus = 1, paymentMethod = ? WHERE id = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, paymentMethod.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, orderId);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc == SQLITE_DONE)
    {
        cout << "\nPayment processed successfully.\n";
        return true;
    }

    return false;
}

bool PaymentService::createReceipt(int orderId, double amount, const string& paymentMethod)
{
    const char* sql = "INSERT INTO receipts(orderId, amount, paymentMethod, status) VALUES(?, ?, ?, 'Paid');";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, orderId);
    sqlite3_bind_double(stmt, 2, amount);
    sqlite3_bind_text(stmt, 3, paymentMethod.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc == SQLITE_DONE)
    {
        cout << "\nReceipt created successfully.\n";
        return true;
    }

    return false;
}

vector<Receipt> PaymentService::getOrderReceipts(int orderId)
{
    vector<Receipt> receipts;
    const char* sql = "SELECT receiptId, orderId, amount, paymentMethod, status FROM receipts WHERE orderId = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return receipts;
    }

    sqlite3_bind_int(stmt, 1, orderId);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        Receipt r;
        r.receiptId = sqlite3_column_int(stmt, 0);
        r.orderId = sqlite3_column_int(stmt, 1);
        r.amount = sqlite3_column_double(stmt, 2);
        r.paymentMethod = (const char*)sqlite3_column_text(stmt, 3);
        r.status = (const char*)sqlite3_column_text(stmt, 4);
        receipts.push_back(r);
    }

    sqlite3_finalize(stmt);
    return receipts;
}

vector<Receipt> PaymentService::getAllReceipts()
{
    vector<Receipt> receipts;
    const char* sql = "SELECT receiptId, orderId, amount, paymentMethod, status FROM receipts;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return receipts;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        Receipt r;
        r.receiptId = sqlite3_column_int(stmt, 0);
        r.orderId = sqlite3_column_int(stmt, 1);
        r.amount = sqlite3_column_double(stmt, 2);
        r.paymentMethod = (const char*)sqlite3_column_text(stmt, 3);
        r.status = (const char*)sqlite3_column_text(stmt, 4);
        receipts.push_back(r);
    }

    sqlite3_finalize(stmt);
    return receipts;
}

double PaymentService::getPlatformRevenue()
{
    double revenue = 0;
    const char* sql = "SELECT SUM(platformProfit) FROM orders WHERE paymentStatus = 1;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return 0;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        revenue = sqlite3_column_double(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return revenue;
}
#include "PaymentService.h"
#include "Utilities.h"
#include <iostream>

using namespace std;

bool PaymentService::processPayment(int orderId, const string& paymentMethod)
{
    const char* sql = "UPDATE orders SET paymentStatus = 1, paymentMethod = ? WHERE id = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, paymentMethod.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, orderId);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc == SQLITE_DONE)
    {
        cout << "\nPayment processed successfully.\n";
        return true;
    }

    return false;
}

bool PaymentService::createReceipt(int orderId, double amount, const string& paymentMethod)
{
    const char* sql = "INSERT INTO receipts(orderId, amount, paymentMethod, status) VALUES(?, ?, ?, 'Paid');";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, orderId);
    sqlite3_bind_double(stmt, 2, amount);
    sqlite3_bind_text(stmt, 3, paymentMethod.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc == SQLITE_DONE)
    {
        cout << "\nReceipt created successfully.\n";
        return true;
    }

    return false;
}

vector<Receipt> PaymentService::getOrderReceipts(int orderId)
{
    vector<Receipt> receipts;
    const char* sql = "SELECT receiptId, orderId, amount, paymentMethod, status FROM receipts WHERE orderId = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return receipts;
    }

    sqlite3_bind_int(stmt, 1, orderId);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        Receipt r;
        r.receiptId = sqlite3_column_int(stmt, 0);
        r.orderId = sqlite3_column_int(stmt, 1);
        r.amount = sqlite3_column_double(stmt, 2);
        r.paymentMethod = (const char*)sqlite3_column_text(stmt, 3);
        r.status = (const char*)sqlite3_column_text(stmt, 4);
        receipts.push_back(r);
    }

    sqlite3_finalize(stmt);
    return receipts;
}

vector<Receipt> PaymentService::getAllReceipts()
{
    vector<Receipt> receipts;
    const char* sql = "SELECT receiptId, orderId, amount, paymentMethod, status FROM receipts;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return receipts;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        Receipt r;
        r.receiptId = sqlite3_column_int(stmt, 0);
        r.orderId = sqlite3_column_int(stmt, 1);
        r.amount = sqlite3_column_double(stmt, 2);
        r.paymentMethod = (const char*)sqlite3_column_text(stmt, 3);
        r.status = (const char*)sqlite3_column_text(stmt, 4);
        receipts.push_back(r);
    }

    sqlite3_finalize(stmt);
    return receipts;
}

double PaymentService::getPlatformRevenue()
{
    double revenue = 0;
    const char* sql = "SELECT SUM(platformProfit) FROM orders WHERE paymentStatus = 1;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return 0;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        revenue = sqlite3_column_double(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return revenue;
}

