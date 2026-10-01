#include "DriverService.h"
#include "Utilities.h"
#include <iostream>

using namespace std;

bool DriverService::registerDriver(const string& name, const string& phone, const string& password)
{
    const char* sql = "INSERT INTO drivers(name, phone, password, available) VALUES(?, ?, ?, 1);";
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
        cerr << "Failed to register driver\n";
        return false;
    }

    cout << "\nDriver registered successfully.\n";
    return true;
}

bool DriverService::driverLogin(const string& phone, const string& password, int& driverId)
{
    const char* sql = "SELECT id, password FROM drivers WHERE phone = ?;";
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
            driverId = sqlite3_column_int(stmt, 0);
            cout << "\nWelcome Driver.\n";
            sqlite3_finalize(stmt);
            return true;
        }
    }

    sqlite3_finalize(stmt);
    cout << "\nLogin failed.\n";
    return false;
}

bool DriverService::updateDriverLocation(int driverId, double latitude, double longitude)
{
    const char* sql = "UPDATE drivers SET latitude = ?, longitude = ? WHERE id = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_double(stmt, 1, latitude);
    sqlite3_bind_double(stmt, 2, longitude);
    sqlite3_bind_int(stmt, 3, driverId);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

vector<PlatformOrder> DriverService::getAvailableOrders()
{
    vector<PlatformOrder> orders;
    const char* sql = "SELECT id, customerId, driverId, commodityTotal, deliveryFee, grandTotal, platformProfit, "
                      "paymentMethod, paymentStatus, orderStatus, orderDate FROM orders WHERE paymentStatus = 1 AND driverId = 0;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return orders;
    }

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

bool DriverService::acceptOrder(int driverId, int orderId)
{
    const char* sql = "UPDATE orders SET driverId = ?, orderStatus = 3 WHERE id = ? AND driverId = 0;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, driverId);
    sqlite3_bind_int(stmt, 2, orderId);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc == SQLITE_DONE)
    {
        cout << "\nOrder accepted successfully.\n";
        return true;
    }

    return false;
}

bool DriverService::pickupOrder(int driverId, int orderId)
{
    const char* sql = "UPDATE orders SET orderStatus = 4 WHERE id = ? AND driverId = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, orderId);
    sqlite3_bind_int(stmt, 2, driverId);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc == SQLITE_DONE)
    {
        cout << "\nOrder picked up successfully.\n";
        return true;
    }

    return false;
}

bool DriverService::startDelivery(int driverId, int orderId)
{
    const char* sql = "UPDATE orders SET orderStatus = 5 WHERE id = ? AND driverId = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, orderId);
    sqlite3_bind_int(stmt, 2, driverId);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc == SQLITE_DONE)
    {
        cout << "\nDelivery started successfully.\n";
        return true;
    }

    return false;
}

bool DriverService::confirmDelivery(int driverId, int orderId)
{
    const char* sql = "UPDATE orders SET orderStatus = 6 WHERE id = ? AND driverId = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, orderId);
    sqlite3_bind_int(stmt, 2, driverId);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc == SQLITE_DONE)
    {
        cout << "\nDelivery confirmed successfully.\n";
        return true;
    }

    return false;
}

vector<PlatformOrder> DriverService::getDriverOrders(int driverId)
{
    vector<PlatformOrder> orders;
    const char* sql = "SELECT id, customerId, driverId, commodityTotal, deliveryFee, grandTotal, platformProfit, "
                      "paymentMethod, paymentStatus, orderStatus, orderDate FROM orders WHERE driverId = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return orders;
    }

    sqlite3_bind_int(stmt, 1, driverId);

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

Skydriver* DriverService::findNearestDriver(double customerLat, double customerLon)
{
    const char* sql = "SELECT id, name, phone, latitude, longitude, available FROM drivers WHERE available = 1;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return nullptr;
    }

    Skydriver* nearest = nullptr;
    double minDistance = 999999;

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        Skydriver d;
        d.id = sqlite3_column_int(stmt, 0);
        d.name = (const char*)sqlite3_column_text(stmt, 1);
        d.phone = (const char*)sqlite3_column_text(stmt, 2);
        d.latitude = sqlite3_column_double(stmt, 3);
        d.longitude = sqlite3_column_double(stmt, 4);
        d.available = sqlite3_column_int(stmt, 5) == 1;

        double distance = calculateDistance(d.latitude, d.longitude, customerLat, customerLon);

        if (distance < minDistance)
        {
            minDistance = distance;
            nearest = new Skydriver(d);
        }
    }

    sqlite3_finalize(stmt);
    return nearest;
}
