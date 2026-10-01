#include "CustomerService.h"
#include "Utilities.h"
#include <iostream>

using namespace std;

bool CustomerService::registerCustomer(const string& name, const string& phone1, const string& phone2,
                                       const string& email, double latitude, double longitude, const string& password)
{
    string mapLink = generateMapsLink(latitude, longitude);

    const char* sql = "INSERT INTO customers(name, phone1, phone2, email, latitude, longitude, mapLink, password, freeDeliveriesRemaining) "
                      "VALUES(?, ?, ?, ?, ?, ?, ?, ?, 2);";
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
    sqlite3_bind_text(stmt, 8, password.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE)
    {
        cerr << "Failed to register customer\n";
        return false;
    }

    cout << "\nCustomer registered successfully.\n";
    cout << "Location Map: " << mapLink << "\n";
    return true;
}

bool CustomerService::customerLogin(const string& phone, const string& password, int& customerId)
{
    const char* sql = "SELECT id, password FROM customers WHERE phone1 = ?;";
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
            customerId = sqlite3_column_int(stmt, 0);
            cout << "\nWelcome Customer.\n";
            sqlite3_finalize(stmt);
            return true;
        }
    }

    sqlite3_finalize(stmt);
    cout << "\nLogin failed.\n";
    return false;
}

vector<Commodity> CustomerService::searchCommodity(const string& keyword)
{
    vector<Commodity> results;
    const char* sql = "SELECT id, sellerId, name, category, description, quantity, sellerPrice, adminMarkup, deliveryFee, photoPath, active "
                      "FROM commodities WHERE name LIKE ? AND active = 1;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return results;
    }

    string searchTerm = "%" + keyword + "%";
    sqlite3_bind_text(stmt, 1, searchTerm.c_str(), -1, SQLITE_STATIC);

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
        results.push_back(c);
    }

    sqlite3_finalize(stmt);
    return results;
}

bool CustomerService::addToCart(int customerId, int commodityId, int quantity)
{
    const char* sql = "INSERT INTO cart(customerId, commodityId, quantity) VALUES(?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, customerId);
    sqlite3_bind_int(stmt, 2, commodityId);
    sqlite3_bind_int(stmt, 3, quantity);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE)
    {
        cerr << "Failed to add to cart\n";
        return false;
    }

    cout << "\nProduct added to cart successfully.\n";
    return true;
}

vector<CartItem> CustomerService::getCustomerCart(int customerId)
{
    vector<CartItem> cart;
    const char* sql = "SELECT customerId, commodityId, quantity FROM cart WHERE customerId = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return cart;
    }

    sqlite3_bind_int(stmt, 1, customerId);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        CartItem item;
        item.customerId = sqlite3_column_int(stmt, 0);
        item.commodityId = sqlite3_column_int(stmt, 1);
        item.quantity = sqlite3_column_int(stmt, 2);
        cart.push_back(item);
    }

    sqlite3_finalize(stmt);
    return cart;
}

bool CustomerService::clearCart(int customerId)
{
    const char* sql = "DELETE FROM cart WHERE customerId = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, customerId);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

bool CustomerService::viewCart(int customerId)
{
    vector<CartItem> cartItems = getCustomerCart(customerId);

    if (cartItems.empty())
    {
        cout << "\nYour cart is empty.\n";
        return false;
    }

    double grandTotal = 0;
    cout << "\n======================\n";
    cout << "YOUR SHOPPING CART\n";
    cout << "========================\n";

    for (const auto& item : cartItems)
    {
        const char* sql = "SELECT name, sellerPrice, adminMarkup FROM commodities WHERE id = ?;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
            continue;

        sqlite3_bind_int(stmt, 1, item.commodityId);

        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            string name = (const char*)sqlite3_column_text(stmt, 0);
            double sellerPrice = sqlite3_column_double(stmt, 1);
            double adminMarkup = sqlite3_column_double(stmt, 2);
            double unitPrice = sellerPrice + adminMarkup;
            double totalPrice = unitPrice * item.quantity;

            cout << "\nCommodity ID: " << item.commodityId << endl;
            cout << "Name: " << name << endl;
            cout << "Quantity: " << item.quantity << endl;
            cout << "Unit Price: UGX " << formatUGX(unitPrice) << endl;
            cout << "Total Price: UGX " << formatUGX(totalPrice) << endl;
            cout << "-----------------------------\n";

            grandTotal += totalPrice;
        }

        sqlite3_finalize(stmt);
    }

    cout << "Cart Total: UGX " << formatUGX(grandTotal) << endl;
    cout << "=================================\n";
    return true;
}

bool CustomerService::placeOrder(int customerId)
{
    vector<CartItem> cartItems = getCustomerCart(customerId);

    if (cartItems.empty())
    {
        cout << "\nYour cart is empty.\n";
        return false;
    }

    const char* insertOrder = "INSERT INTO orders(customerId, driverId, commodityTotal, deliveryFee, grandTotal, platformProfit, "
                              "paymentMethod, paymentStatus, orderStatus, orderDate) "
                              "VALUES(?, 0, 0, 0, 0, 0, 'Not Paid', 0, 0, 'Today');";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, insertOrder, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, customerId);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE)
    {
        cerr << "Failed to create order\n";
        return false;
    }

    int orderId = sqlite3_last_insert_rowid(db);

    for (const auto& cartItem : cartItems)
    {
        const char* getCommodity = "SELECT sellerId, name, sellerPrice, adminMarkup, deliveryFee FROM commodities WHERE id = ?;";
        sqlite3_stmt* stmtCom;

        if (sqlite3_prepare_v2(db, getCommodity, -1, &stmtCom, nullptr) != SQLITE_OK)
            continue;

        sqlite3_bind_int(stmtCom, 1, cartItem.commodityId);

        if (sqlite3_step(stmtCom) == SQLITE_ROW)
        {
            int sellerId = sqlite3_column_int(stmtCom, 0);
            string commodityName = (const char*)sqlite3_column_text(stmtCom, 1);
            double sellerPrice = sqlite3_column_double(stmtCom, 2);
            double adminMarkup = sqlite3_column_double(stmtCom, 3);
            double deliveryFee = sqlite3_column_double(stmtCom, 4);

            double customerUnitPrice = sellerPrice + adminMarkup;
            double totalPrice = customerUnitPrice * cartItem.quantity;

            const char* insertItem = "INSERT INTO orderItems(orderId, commodityId, sellerId, commodityName, quantity, "
                                     "sellerPrice, customerUnitPrice, totalPrice) VALUES(?, ?, ?, ?, ?, ?, ?, ?);";
            sqlite3_stmt* stmtItem;

            if (sqlite3_prepare_v2(db, insertItem, -1, &stmtItem, nullptr) == SQLITE_OK)
            {
                sqlite3_bind_int(stmtItem, 1, orderId);
                sqlite3_bind_int(stmtItem, 2, cartItem.commodityId);
                sqlite3_bind_int(stmtItem, 3, sellerId);
                sqlite3_bind_text(stmtItem, 4, commodityName.c_str(), -1, SQLITE_STATIC);
                sqlite3_bind_int(stmtItem, 5, cartItem.quantity);
                sqlite3_bind_double(stmtItem, 6, sellerPrice);
                sqlite3_bind_double(stmtItem, 7, customerUnitPrice);
                sqlite3_bind_double(stmtItem, 8, totalPrice);

                sqlite3_step(stmtItem);
                sqlite3_finalize(stmtItem);
            }

            const char* updateQty = "UPDATE commodities SET quantity = quantity - ? WHERE id = ?;";
            sqlite3_stmt* stmtQty;

            if (sqlite3_prepare_v2(db, updateQty, -1, &stmtQty, nullptr) == SQLITE_OK)
            {
                sqlite3_bind_int(stmtQty, 1, cartItem.quantity);
                sqlite3_bind_int(stmtQty, 2, cartItem.commodityId);
                sqlite3_step(stmtQty);
                sqlite3_finalize(stmtQty);
            }
        }

        sqlite3_finalize(stmtCom);
    }

    clearCart(customerId);

    cout << "\nOrder placed successfully.\n";
    cout << "Order ID: " << orderId << endl;
    return true;
}

vector<PlatformOrder> CustomerService::getCustomerOrders(int customerId)
{
    vector<PlatformOrder> orders;
    const char* sql = "SELECT id, customerId, driverId, commodityTotal, deliveryFee, grandTotal, platformProfit, "
                      "paymentMethod, paymentStatus, orderStatus, orderDate FROM orders WHERE customerId = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return orders;
    }

    sqlite3_bind_int(stmt, 1, customerId);

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

bool CustomerService::cancelOrder(int customerId, int orderId)
{
    const char* sql = "UPDATE orders SET orderStatus = 8 WHERE id = ? AND customerId = ? AND driverId = 0;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, orderId);
    sqlite3_bind_int(stmt, 2, customerId);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE)
    {
        cout << "\nCannot cancel this order.\n";
        return false;
    }

    cout << "\nOrder cancelled successfully.\n";
    return true;
}
