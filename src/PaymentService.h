#ifndef PAYMENT_SERVICE_H
#define PAYMENT_SERVICE_H

#include <sqlite3.h>
#include <vector>
#include "Models.h"

using namespace std;

class PaymentService
{
private:
    sqlite3* db;

public:
    PaymentService(sqlite3* database) : db(database) {}

    bool processPayment(int orderId, const string& paymentMethod);
    bool createReceipt(int orderId, double amount, const string& paymentMethod);
    vector<Receipt> getOrderReceipts(int orderId);
    vector<Receipt> getAllReceipts();
    double getPlatformRevenue();
};

#endif
