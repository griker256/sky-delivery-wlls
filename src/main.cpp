#include <iostream>
#include <string>
#include "Database.h"
#include "AdminService.h"
#include "CustomerService.h"
#include "SellerService.h"
#include "DriverService.h"
#include "PaymentService.h"
#include "Utilities.h"

using namespace std;

int main()
{
    Database db;
    
    if (!db.connect("sky_delivery.db"))
    {
        cerr << "Failed to connect to database\n";
        return 1;
    }

    if (!db.createTables())
    {
        cerr << "Failed to create tables\n";
        return 1;
    }

    sqlite3* connection = db.getConnection();

    AdminService adminService(connection);
    CustomerService customerService(connection);
    SellerService sellerService(connection);
    DriverService driverService(connection);
    PaymentService paymentService(connection);

    int choice = 0;
    bool running = true;

    while (running)
    {
        cout << "\n========================================\n";
        cout << "   SKY DELIVERY WLLS - MAIN MENU\n";
        cout << "========================================\n";
        cout << "1. Admin\n";
        cout << "2. Customer\n";
        cout << "3. Seller\n";
        cout << "4. Driver\n";
        cout << "5. Exit\n";
        cout << "Choose: ";
        cin >> choice;
        clearInput();

        switch (choice)
        {
            case 1: // Admin Menu
            {
                int adminChoice;
                bool adminLoggedIn = false;

                cout << "\n1. Register Admin\n2. Login Admin\nChoose: ";
                cin >> adminChoice;
                clearInput();

                if (adminChoice == 1)
                {
                    string name, phone, password;
                    cout << "Admin Name: ";
                    getline(cin, name);
                    cout << "Phone: ";
                    getline(cin, phone);
                    cout << "Password: ";
                    getline(cin, password);

                    adminService.registerAdmin(name, phone, password);
                }
                else if (adminChoice == 2)
                {
                    string password;
                    cout << "Password: ";
                    getline(cin, password);

                    if (adminService.adminLogin(password))
                    {
                        adminLoggedIn = true;

                        while (adminLoggedIn)
                        {
                            cout << "\nAdmin Menu:\n";
                            cout << "1. Set Admin Markup\n";
                            cout << "2. View Platform Revenue\n";
                            cout << "3. Logout\n";
                            cout << "Choose: ";
                            cin >> adminChoice;
                            clearInput();

                            if (adminChoice == 1)
                            {
                                int commodityId;
                                double markup;
                                cout << "Commodity ID: ";
                                cin >> commodityId;
                                cout << "Markup Amount: ";
                                cin >> markup;
                                clearInput();

                                adminService.updateAdminMarkup(commodityId, markup);
                            }
                            else if (adminChoice == 2)
                            {
                                double revenue = paymentService.getPlatformRevenue();
                                cout << "\nPlatform Revenue: UGX " << formatUGX(revenue) << endl;
                            }
                            else if (adminChoice == 3)
                            {
                                adminLoggedIn = false;
                            }
                        }
                    }
                }
                break;
            }

            case 2: // Customer Menu
            {
                int customerChoice;
                int customerId = 0;
                bool customerLoggedIn = false;

                cout << "\n1. Register Customer\n2. Login Customer\nChoose: ";
                cin >> customerChoice;
                clearInput();

                if (customerChoice == 1)
                {
                    string name, phone1, phone2, email, password;
                    double latitude, longitude;

                    cout << "Name: ";
                    getline(cin, name);
                    cout << "Phone 1: ";
                    getline(cin, phone1);
                    cout << "Phone 2: ";
                    getline(cin, phone2);
                    cout << "Email: ";
                    getline(cin, email);
                    cout << "Latitude: ";
                    cin >> latitude;
                    cout << "Longitude: ";
                    cin >> longitude;
                    clearInput();
                    cout << "Password: ";
                    getline(cin, password);

                    customerService.registerCustomer(name, phone1, phone2, email, latitude, longitude, password);
                }
                else if (customerChoice == 2)
                {
                    string phone, password;
                    cout << "Phone: ";
                    getline(cin, phone);
                    cout << "Password: ";
                    getline(cin, password);

                    if (customerService.customerLogin(phone, password, customerId))
                    {
                        customerLoggedIn = true;

                        while (customerLoggedIn)
                        {
                            cout << "\nCustomer Menu:\n";
                            cout << "1. Search Products\n";
                            cout << "2. View Cart\n";
                            cout << "3. Add to Cart\n";
                            cout << "4. Place Order\n";
                            cout << "5. View My Orders\n";
                            cout << "6. Logout\n";
                            cout << "Choose: ";
                            cin >> customerChoice;
                            clearInput();

                            if (customerChoice == 1)
                            {
                                string keyword;
                                cout << "Search Keyword: ";
                                getline(cin, keyword);

                                vector<Commodity> results = customerService.searchCommodity(keyword);
                                cout << "\n===== SEARCH RESULTS =====\n";
                                for (const auto& c : results)
                                {
                                    cout << "ID: " << c.id << " | Name: " << c.name << " | Price: UGX "
                                         << formatUGX(c.sellerPrice + c.adminMarkup) << " | Stock: " << c.quantity << endl;
                                }
                            }
                            else if (customerChoice == 2)
                            {
                                customerService.viewCart(customerId);
                            }
                            else if (customerChoice == 3)
                            {
                                int commodityId, quantity;
                                cout << "Commodity ID: ";
                                cin >> commodityId;
                                cout << "Quantity: ";
                                cin >> quantity;
                                clearInput();

                                customerService.addToCart(customerId, commodityId, quantity);
                            }
                            else if (customerChoice == 4)
                            {
                                if (customerService.placeOrder(customerId))
                                {
                                    cout << "Proceed to Payment? (1=Yes, 0=No): ";
                                    int payChoice;
                                    cin >> payChoice;
                                    clearInput();

                                    if (payChoice == 1)
                                    {
                                        int orderId;
                                        cout << "Enter Order ID: ";
                                        cin >> orderId;
                                        clearInput();

                                        string paymentMethod;
                                        cout << "Payment Method (Mobile Money/Card/Cash): ";
                                        getline(cin, paymentMethod);

                                        if (paymentService.processPayment(orderId, paymentMethod))
                                        {
                                            // Get order total
                                            const char* sql = "SELECT grandTotal FROM orders WHERE id = ?;";
                                            sqlite3_stmt* stmt;
                                            sqlite3_prepare_v2(connection, sql, -1, &stmt, nullptr);
                                            sqlite3_bind_int(stmt, 1, orderId);

                                            if (sqlite3_step(stmt) == SQLITE_ROW)
                                            {
                                                double amount = sqlite3_column_double(stmt, 0);
                                                paymentService.createReceipt(orderId, amount, paymentMethod);
                                            }
                                            sqlite3_finalize(stmt);
                                        }
                                    }
                                }
                            }
                            else if (customerChoice == 5)
                            {
                                vector<PlatformOrder> orders = customerService.getCustomerOrders(customerId);
                                cout << "\n===== MY ORDERS =====\n";
                                for (const auto& o : orders)
                                {
                                    cout << "Order ID: " << o.id << " | Status: " << orderStatusToString(o.orderStatus)
                                         << " | Total: UGX " << formatUGX(o.grandTotal) << endl;
                                }
                            }
                            else if (customerChoice == 6)
                            {
                                customerLoggedIn = false;
                            }
                        }
                    }
                }
                break;
            }

            case 3: // Seller Menu
            {
                int sellerChoice;
                int sellerId = 0;
                bool sellerLoggedIn = false;

                cout << "\n1. Register Seller\n2. Login Seller\nChoose: ";
                cin >> sellerChoice;
                clearInput();

                if (sellerChoice == 1)
                {
                    string name, phone1, phone2, email, nin, password;
                    double latitude, longitude;

                    cout << "Business Name: ";
                    getline(cin, name);
                    cout << "Phone 1: ";
                    getline(cin, phone1);
                    cout << "Phone 2: ";
                    getline(cin, phone2);
                    cout << "Email: ";
                    getline(cin, email);
                    cout << "NIN: ";
                    getline(cin, nin);
                    cout << "Latitude: ";
                    cin >> latitude;
                    cout << "Longitude: ";
                    cin >> longitude;
                    clearInput();
                    cout << "Password: ";
                    getline(cin, password);

                    sellerService.registerSeller(name, phone1, phone2, email, latitude, longitude, nin, password);
                }
                else if (sellerChoice == 2)
                {
                    string phone, password;
                    cout << "Phone: ";
                    getline(cin, phone);
                    cout << "Password: ";
                    getline(cin, password);

                    if (sellerService.sellerLogin(phone, password, sellerId))
                    {
                        sellerLoggedIn = true;

                        while (sellerLoggedIn)
                        {
                            cout << "\nSeller Menu:\n";
                            cout << "1. Add Commodity\n";
                            cout << "2. View My Products\n";
                            cout << "3. View My Orders\n";
                            cout << "4. View My Earnings\n";
                            cout << "5. Logout\n";
                            cout << "Choose: ";
                            cin >> sellerChoice;
                            clearInput();

                            if (sellerChoice == 1)
                            {
                                string name, category, description, photoPath;
                                int quantity;
                                double price;

                                cout << "Product Name: ";
                                getline(cin, name);
                                cout << "Category: ";
                                getline(cin, category);
                                cout << "Description: ";
                                getline(cin, description);
                                cout << "Quantity: ";
                                cin >> quantity;
                                cout << "Price: ";
                                cin >> price;
                                clearInput();
                                cout << "Photo Path: ";
                                getline(cin, photoPath);

                                sellerService.addCommodity(sellerId, name, category, description, quantity, price, photoPath);
                            }
                            else if (sellerChoice == 2)
                            {
                                vector<Commodity> commodities = sellerService.getSellerCommodities(sellerId);
                                cout << "\n===== MY PRODUCTS =====\n";
                                for (const auto& c : commodities)
                                {
                                    cout << "ID: " << c.id << " | Name: " << c.name << " | Price: UGX "
                                         << formatUGX(c.sellerPrice) << " | Stock: " << c.quantity << endl;
                                }
                            }
                            else if (sellerChoice == 3)
                            {
                                vector<PlatformOrder> orders = sellerService.getSellerOrders(sellerId);
                                cout << "\n===== MY ORDERS =====\n";
                                for (const auto& o : orders)
                                {
                                    cout << "Order ID: " << o.id << " | Status: " << orderStatusToString(o.orderStatus)
                                         << " | Total: UGX " << formatUGX(o.grandTotal) << endl;
                                }
                            }
                            else if (sellerChoice == 4)
                            {
                                double earnings = sellerService.getSellerEarnings(sellerId);
                                cout << "\nYour Earnings: UGX " << formatUGX(earnings) << endl;
                            }
                            else if (sellerChoice == 5)
                            {
                                sellerLoggedIn = false;
                            }
                        }
                    }
                }
                break;
            }

            case 4: // Driver Menu
            {
                int driverChoice;
                int driverId = 0;
                bool driverLoggedIn = false;

                cout << "\n1. Register Driver\n2. Login Driver\nChoose: ";
                cin >> driverChoice;
                clearInput();

                if (driverChoice == 1)
                {
                    string name, phone, password;
                    cout << "Name: ";
                    getline(cin, name);
                    cout << "Phone: ";
                    getline(cin, phone);
                    cout << "Password: ";
                    getline(cin, password);

                    driverService.registerDriver(name, phone, password);
                }
                else if (driverChoice == 2)
                {
                    string phone, password;
                    cout << "Phone: ";
                    getline(cin, phone);
                    cout << "Password: ";
                    getline(cin, password);

                    if (driverService.driverLogin(phone, password, driverId))
                    {
                        driverLoggedIn = true;

                        while (driverLoggedIn)
                        {
                            cout << "\nDriver Menu:\n";
                            cout << "1. Update Location\n";
                            cout << "2. View Available Orders\n";
                            cout << "3. Accept Order\n";
                            cout << "4. Pickup Order\n";
                            cout << "5. Start Delivery\n";
                            cout << "6. Confirm Delivery\n";
                            cout << "7. View My Orders\n";
                            cout << "8. Logout\n";
                            cout << "Choose: ";
                            cin >> driverChoice;
                            clearInput();

                            if (driverChoice == 1)
                            {
                                double latitude, longitude;
                                cout << "Current Latitude: ";
                                cin >> latitude;
                                cout << "Current Longitude: ";
                                cin >> longitude;
                                clearInput();

                                driverService.updateDriverLocation(driverId, latitude, longitude);
                                cout << "\nLocation updated.\n";
                            }
                            else if (driverChoice == 2)
                            {
                                vector<PlatformOrder> orders = driverService.getAvailableOrders();
                                cout << "\n===== AVAILABLE ORDERS =====\n";
                                for (const auto& o : orders)
                                {
                                    cout << "Order ID: " << o.id << " | Total: UGX " << formatUGX(o.grandTotal) << endl;
                                }
                            }
                            else if (driverChoice == 3)
                            {
                                int orderId;
                                cout << "Order ID to Accept: ";
                                cin >> orderId;
                                clearInput();

                                driverService.acceptOrder(driverId, orderId);
                            }
                            else if (driverChoice == 4)
                            {
                                int orderId;
                                cout << "Order ID to Pickup: ";
                                cin >> orderId;
                                clearInput();

                                driverService.pickupOrder(driverId, orderId);
                            }
                            else if (driverChoice == 5)
                            {
                                int orderId;
                                cout << "Order ID: ";
                                cin >> orderId;
                                clearInput();

                                driverService.startDelivery(driverId, orderId);
                            }
                            else if (driverChoice == 6)
                            {
                                int orderId;
                                cout << "Order ID to Confirm: ";
                                cin >> orderId;
                                clearInput();

                                driverService.confirmDelivery(driverId, orderId);
                            }
                            else if (driverChoice == 7)
                            {
                                vector<PlatformOrder> orders = driverService.getDriverOrders(driverId);
                                cout << "\n===== MY DELIVERIES =====\n";
                                for (const auto& o : orders)
                                {
                                    cout << "Order ID: " << o.id << " | Status: " << orderStatusToString(o.orderStatus) << endl;
                                }
                            }
                            else if (driverChoice == 8)
                            {
                                driverLoggedIn = false;
                            }
                        }
                    }
                }
                break;
            }

            case 5:
                running = false;
                cout << "\nGoodbye!\n";
                break;

            default:
                cout << "\nInvalid choice.\n";
        }
    }

    db.close();
    return 0;
}
