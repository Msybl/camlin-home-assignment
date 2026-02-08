#include "database.h"
#include <sqlite3.h>
#include <iostream>

const std::string DB_PATH = "data/wallet.db";

bool initDatabase() {
    sqlite3* db;
    char* errMsg = nullptr;

    // Open database (create if not exist)
    int rc = sqlite3_open(DB_PATH.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // Create SQL table
    const char* sql = 
        "CREATE TABLE IF NOT EXISTS wallet ("
        "    currency_code TEXT PRIMARY KEY,"
        "    amount REAL NOT NULL"
        ");";
        
    // Execute SQL
    rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return false;
    }
    
    sqlite3_close(db);

    std::cout << "Database initialized successfully" << std::endl;
    return true;
}
