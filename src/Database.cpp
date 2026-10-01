#include "Database.h"
#include <iostream>

Database::Database() : db(nullptr) {}

Database::~Database()
{
    close();
}

bool Database::connect(const string& dbPath)
{
    int rc = sqlite3_open(dbPath.c_str(), &db);

    if (rc)
    {
        cerr << "Cannot open database: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    cout << "Connected to SQLite database\n";
    return true;
}

bool Database::createTables()
{
    const char* sql =
        "CREATE TABLE IF NOT EXISTS admins("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "phone TEXT NOT NULL,"
        "password TEXT NOT NULL);"

        "CREATE TABLE IF NOT EXISTS drivers("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "phone TEXT NOT NULL,"
        "password TEXT NOT NULL,"
        "available INTEGER DEFAULT 1,"
        "latitude REAL DEFAULT 0,"
        "longitude REAL DEFAULT 0);"

        "CREATE TABLE IF NOT EXISTS customers("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "phone1 TEXT NOT NULL,"
        "phone2 TEXT,"
        "email TEXT,"
        "latitude REAL NOT NULL,"
        "longitude REAL NOT NULL,"
        "mapLink TEXT,"
        "password TEXT NOT NULL,"
        "freeDeliveriesRemaining INTEGER DEFAULT 2);"

        "CREATE TABLE IF NOT EXISTS sellers("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "phone1 TEXT NOT NULL,"
        "phone2 TEXT,"
        "email TEXT,"
        "latitude REAL NOT NULL,"
        "longitude REAL NOT NULL,"
        "mapLink TEXT,"
        "nin TEXT NOT NULL,"
        "password TEXT NOT NULL);"

        "CREATE TABLE IF NOT EXISTS commodities("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "sellerId INTEGER NOT NULL,"
        "name TEXT NOT NULL,"
        "category TEXT,"
        "description TEXT,"
        "quantity INTEGER,"
        "sellerPrice REAL,"
        "adminMarkup REAL DEFAULT 0,"
        "deliveryFee REAL DEFAULT 0,"
        "photoPath TEXT,"
        "active INTEGER DEFAULT 1,"
        "FOREIGN KEY(sellerId) REFERENCES sellers(id));"

        "CREATE TABLE IF NOT EXISTS orders("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "customerId INTEGER NOT NULL,"
        "driverId INTEGER DEFAULT 0,"
        "commodityTotal REAL,"
        "deliveryFee REAL,"
        "grandTotal REAL,"
        "platformProfit REAL DEFAULT 0,"
        "paymentMethod TEXT,"
        "paymentStatus INTEGER DEFAULT 0,"
        "orderStatus INTEGER DEFAULT 0,"
        "orderDate TEXT,"
        "FOREIGN KEY(customerId) REFERENCES customers(id),"
        "FOREIGN KEY(driverId) REFERENCES drivers(id));"

        "CREATE TABLE IF NOT EXISTS orderItems("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "orderId INTEGER NOT NULL,"
        "commodityId INTEGER NOT NULL,"
        "sellerId INTEGER NOT NULL,"
        "commodityName TEXT,"
        "quantity INTEGER,"
        "sellerPrice REAL,"
        "customerUnitPrice REAL,"
        "totalPrice REAL,"
        "FOREIGN KEY(orderId) REFERENCES orders(id),"
        "FOREIGN KEY(commodityId) REFERENCES commodities(id),"
        "FOREIGN KEY(sellerId) REFERENCES sellers(id));"

        "CREATE TABLE IF NOT EXISTS receipts("
        "receiptId INTEGER PRIMARY KEY AUTOINCREMENT,"
        "orderId INTEGER NOT NULL,"
        "amount REAL,"
        "paymentMethod TEXT,"
        "status TEXT,"
        "FOREIGN KEY(orderId) REFERENCES orders(id));"

        "CREATE TABLE IF NOT EXISTS cart("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "customerId INTEGER NOT NULL,"
        "commodityId INTEGER NOT NULL,"
        "quantity INTEGER,"
        "FOREIGN KEY(customerId) REFERENCES customers(id),"
        "FOREIGN KEY(commodityId) REFERENCES commodities(id));";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);

    if (rc != SQLITE_OK)
    {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }

    cout << "Tables created successfully\n";
    return true;
}

sqlite3* Database::getConnection()
{
    return db;
}

void Database::close()
{
    if (db)
    {
        sqlite3_close(db);
        db = nullptr;
    }
}
