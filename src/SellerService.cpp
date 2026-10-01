#include "SellerService.h"
#include "Utilities.h"
#include <iostream>

using namespace std;

bool SellerService::registerSeller(const string& name, const string& phone1, const string& phone2,
                                   const string& email, double latitude, double longitude,
                                   const string& nin, const string& password)
{
    string mapLink = generateMapsLink(latitude, longitude);

    const char* sql = "INSERT INTO sellers(name, phone1, phone2, email, latitude, longitude, mapLink, nin, password) "
                      "VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, phone1.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, phone2.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, email.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 5, latitude);
    sqlite3_bind_double(stmt, 6, longitude);
    sqlite3_bind_text(stmt, 7, mapLink.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 8, nin.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 9, password.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE)
    {
        cerr << "Failed to register seller\n";
        return false;
    }

    cout << "\nSeller registered successfully.\n";
    return true;
}

bool SellerService::sellerLogin(const string& phone, const string& password, int& sellerId)
{
    const char* sql = "SELECT id, password FROM sellers WHERE phone1 = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, phone.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        string storedPassword = (const char*)sqlite3_column_text(stmt, 1);

        if (storedPassword == password)
        {
            sellerId = sqlite3_column_int(stmt, 0);
            cout << "\nWelcome Seller.\n";
            sqlite3_finalize(stmt);
            return true;
        }
    }

    sqlite3_finalize(stmt);
    cout << "\nLogin failed.\n";
    return false;
}

bool SellerService::addCommodity(int sellerId, const string& name, const string& category,
                                const string& description, int quantity, double sellerPrice,
                                const string& photoPath)
{
    const char* sql = "INSERT INTO commodities(sellerId, name, category, description, quantity, sellerPrice, photoPath, active) "
                      "VALUES(?, ?, ?, ?, ?, ?, ?, 1);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, sellerId);
    sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, category.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, description.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 5, quantity);
    sqlite3_bind_double(stmt, 6, sellerPrice);
    sqlite3_bind_text(stmt, 7, photoPath.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE)
    {
        cerr << "Failed to add commodity\n";
        return false;
    }

    cout << "\nCommodity added successfully.\n";
    return true;
}

vector<Commodity> SellerService::getSellerCommodities(int sellerId)
{
    vector<Commodity> commodities;
    const char* sql = "SELECT id, sellerId, name, category, description, quantity, sellerPrice, adminMarkup, deliveryFee, photoPath, active "
                      "FROM commodities WHERE sellerId = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return commodities;
    }

    sqlite3_bind_int(stmt, 1, sellerId);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        Commodity c;
        c.id = sqlite3_column_int(stmt, 0);
        c.sellerId = sqlite3_column_int(stmt, 1);
        c.name = (const char*)sqlite3_column_text(stmt, 2);
        c.category = (const char*)sqlite3_column_text(stmt, 3);
        c.description = (const char*)sqlite3_column_text(stmt, 4);
        c.quantity = sqlite3_column_int(stmt, 5);
        c.sellerPrice = sqlite3_column_double(stmt, 6);
        c.adminMarkup = sqlite3_column_double(stmt, 7);
        c.deliveryFee = sqlite3_column_double(stmt, 8);
        c.photoPath = (const char*)sqlite3_column_text(stmt, 9);
        c.active = sqlite3_column_int(stmt, 10) == 1;
        commodities.push_back(c);
    }

    sqlite3_finalize(stmt);
    return commodities;
}

vector<PlatformOrder> SellerService::getSellerOrders(int sellerId)
{
    vector<PlatformOrder> orders;
    const char* sql = "SELECT DISTINCT o.id, o.customerId, o.driverId, o.commodityTotal, o.deliveryFee, o.grandTotal, o.platformProfit, "
                      "o.paymentMethod, o.paymentStatus, o.orderStatus, o.orderDate FROM orders o "
                      "JOIN orderItems oi ON o.id = oi.orderId WHERE oi.sellerId = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return orders;
    }

    sqlite3_bind_int(stmt, 1, sellerId);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        PlatformOrder order;
        order.id = sqlite3_column_int(stmt, 0);
        order.customerId = sqlite3_column_int(stmt, 1);
        order.driverId = sqlite3_column_int(stmt, 2);
        order.commodityTotal = sqlite3_column_double(stmt, 3);
        order.deliveryFee = sqlite3_column_double(stmt, 4);
        order.grandTotal = sqlite3_column_double(stmt, 5);
        order.platformProfit = sqlite3_column_double(stmt, 6);
        order.paymentMethod = (const char*)sqlite3_column_text(stmt, 7);
        order.paymentStatus = (PaymentStatus)sqlite3_column_int(stmt, 8);
        order.orderStatus = (OrderStatus)sqlite3_column_int(stmt, 9);
        order.orderDate = (const char*)sqlite3_column_text(stmt, 10);
        orders.push_back(order);
    }

    sqlite3_finalize(stmt);
    return orders;
}

double SellerService::getSellerEarnings(int sellerId)
{
    double earnings = 0;
    const char* sql = "SELECT SUM(oi.sellerPrice * oi.quantity) as total FROM orderItems oi "
                      "JOIN orders o ON oi.orderId = o.id WHERE oi.sellerId = ? AND o.paymentStatus = 1;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return 0;
    }

    sqlite3_bind_int(stmt, 1, sellerId);

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        earnings = sqlite3_column_double(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return earnings;
}
