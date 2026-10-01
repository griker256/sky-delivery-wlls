#ifndef DRIVER_SERVICE_H
#define DRIVER_SERVICE_H

#include <sqlite3.h>
#include <vector>
#include "Models.h"

using namespace std;

class DriverService
{
private:
    sqlite3* db;

public:
    DriverService(sqlite3* database) : db(database) {}

    bool registerDriver(const string& name, const string& phone, const string& password);
    bool driverLogin(const string& phone, const string& password, int& driverId);
    bool updateDriverLocation(int driverId, double latitude, double longitude);
    vector<PlatformOrder> getAvailableOrders();
    bool acceptOrder(int driverId, int orderId);
    bool pickupOrder(int driverId, int orderId);
    bool startDelivery(int driverId, int orderId);
    bool confirmDelivery(int driverId, int orderId);
    vector<PlatformOrder> getDriverOrders(int driverId);
    Skydriver* findNearestDriver(double customerLat, double customerLon);
};

#endif
