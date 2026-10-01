#ifndef SELLER_SERVICE_H
#define SELLER_SERVICE_H

#include <sqlite3.h>
#include <vector>
#include "Models.h"

using namespace std;

class SellerService
{
private:
    sqlite3* db;

public:
    SellerService(sqlite3* database) : db(database) {}

    bool registerSeller(const string& name, const string& phone1, const string& phone2,
                       const string& email, double latitude, double longitude,
                       const string& nin, const string& password);
    bool sellerLogin(const string& phone, const string& password, int& sellerId);
    bool addCommodity(int sellerId, const string& name, const string& category,
                     const string& description, int quantity, double sellerPrice,
                     const string& photoPath);
    vector<Commodity> getSellerCommodities(int sellerId);
    vector<PlatformOrder> getSellerOrders(int sellerId);
    double getSellerEarnings(int sellerId);
};

#endif
