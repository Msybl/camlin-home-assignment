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

bool loadWalletFromDB(std::map<std::string, double>& wallet) {
    sqlite3* db;
    sqlite3_stmt* stmt;
    
    // Open database
    int rc = sqlite3_open(DB_PATH.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    
    // Prepare SELECT query
    const char* sql = "SELECT currency_code, amount FROM wallet";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return false;
    }
    
    wallet.clear();
    
    // Go through results
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string currency = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        double amount = sqlite3_column_double(stmt, 1);
        wallet[currency] = amount;
        std::cout << "Loaded: " << currency << " = " << amount << std::endl;
    }
    
    // Clean
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    
    std::cout << "Loaded " << wallet.size() << " currencies from database" << std::endl;
    return true;
}
