#ifndef MODELS_H
#define MODELS_H

#include <string>
#include <vector>

using namespace std;

enum class OrderStatus
{
    WaitingForPayment = 0,
    Paid = 1,
    Preparing = 2,
    Assigned = 3,
    PickedUp = 4,
    OnDelivery = 5,
    Delivered = 6,
    Completed = 7,
    Cancelled = 8
};

enum class PaymentStatus
{
    Pending = 0,
    Paid = 1,
    Failed = 2,
    Refunded = 3
};

struct Admin
{
    int id;
    string name;
    string phone;
    string password;
};

struct Skydriver
{
    int id;
    string name;
    string phone;
    string password;
    bool available;
    double latitude;
    double longitude;
};

struct Customer
{
    int id;
    string name;
    string phone1;
    string phone2;
    string email;
    double latitude;
    double longitude;
    string mapLink;
    string password;
    int freeDeliveriesRemaining;
};

struct Seller
{
    int id;
    string name;
    string phone1;
    string phone2;
    string email;
    double latitude;
    double longitude;
    string mapLink;
    string nin;
    string password;
};

struct Commodity
{
    int id;
    int sellerId;
    string name;
    string category;
    string description;
    int quantity;
    double sellerPrice;
    double adminMarkup;
    double deliveryFee;
    string photoPath;
    bool active;
};

struct OrderItem
{
    int commodityId;
    int sellerId;
    string commodityName;
    int quantity;
    double sellerPrice;
    double customerUnitPrice;
    double totalPrice;
};

struct PlatformOrder
{
    int id;
    int customerId;
    int driverId;
    vector<OrderItem> items;
    double commodityTotal;
    double deliveryFee;
    double grandTotal;
    double platformProfit;
    string paymentMethod;
    PaymentStatus paymentStatus;
    OrderStatus orderStatus;
    string orderDate;
};

struct Receipt
{
    int receiptId;
    int orderId;
    double amount;
    string paymentMethod;
    string status;
};

struct CartItem
{
    int customerId;
    int commodityId;
    int quantity;
};

#endif
