#ifndef CUSTOMER_SERVICE_H
#define CUSTOMER_SERVICE_H

#include <sqlite3.h>
#include <vector>
#include "Models.h"

using namespace std;

class CustomerService
{
private:
    sqlite3* db;

public:
    CustomerService(sqlite3* database) : db(database) {}

    bool registerCustomer(const string& name, const string& phone1, const string& phone2,
                         const string& email, double latitude, double longitude, const string& password);
    bool customerLogin(const string& phone, const string& password, int& customerId);
    vector<Commodity> searchCommodity(const string& keyword);
    bool addToCart(int customerId, int commodityId, int quantity);
    vector<CartItem> getCustomerCart(int customerId);
    bool clearCart(int customerId);
    bool placeOrder(int customerId);
    vector<PlatformOrder> getCustomerOrders(int customerId);
    bool cancelOrder(int customerId, int orderId);
    bool viewCart(int customerId);
};

#endif
