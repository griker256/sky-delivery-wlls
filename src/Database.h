#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <iostream>
#include <vector>
#include "Models.h"

using namespace std;

class Database
{
private:
    sqlite3* db;

public:
    Database();
    ~Database();
    bool connect(const string& dbPath);
    bool createTables();
    sqlite3* getConnection();
    void close();
};

#endif
