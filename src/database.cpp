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
        "    user_id TEXT NOT NULL,"
        "    currency_code TEXT NOT NULL,"
        "    amount REAL NOT NULL,"
        "    PRIMARY KEY (user_id, currency_code)"
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

bool loadWalletFromDB(const std::string& user_id, std::map<std::string, double>& wallet) {
    sqlite3* db;
    sqlite3_stmt* stmt;
    
    // Open database
    int rc = sqlite3_open(DB_PATH.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    
    // Prepare SELECT query
    const char* sql = "SELECT currency_code, amount FROM wallet WHERE user_id = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return false;
    }
    
    // Add user_id
    sqlite3_bind_text(stmt, 1, user_id.c_str(), -1, SQLITE_TRANSIENT);

    wallet.clear();
    
    // Go through results
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string currency = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        double amount = sqlite3_column_double(stmt, 1);
        wallet[currency] = amount;
        std::cout << "Loaded for " << user_id << ": " << currency << " = " << amount << std::endl;
    }
    
    // Clean
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    
    std::cout << "Loaded " << wallet.size() << " currencies for user " << user_id << std::endl;
    return true;
}

bool saveCurrencyToDB(const std::string& user_id, const std::string& currency, double amount) {
    sqlite3* db;
    sqlite3_stmt* stmt;
    
    // Open database
    int rc = sqlite3_open(DB_PATH.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    
    // Prepare query
    const char* sql = "REPLACE INTO wallet (user_id, currency_code, amount) VALUES (?, ?, ?)";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return false;
    }
    
    // Add parameters
    sqlite3_bind_text(stmt, 1, user_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, currency.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, amount);
    
    // Execute
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to execute: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return false;
    }
    
    // Clean
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    std::cout << "Saved to DB for " << user_id << ": " << currency << " = " << amount << std::endl;
    return true;
}

bool deleteCurrencyFromDB(const std::string& user_id, const std::string& currency) {
    sqlite3* db;
    sqlite3_stmt* stmt;
    
    // Open database
    int rc = sqlite3_open(DB_PATH.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    
    // Prepare DELETE query
    const char* sql = "DELETE FROM wallet WHERE user_id = ? AND currency_code = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return false;
    }
    
    // Add parameter
    sqlite3_bind_text(stmt, 1, user_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, currency.c_str(), -1, SQLITE_TRANSIENT);
    
    // Execute
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to execute: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return false;
    }
    
    // Clean
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    std::cout << "Deleted from DB for " << user_id << ": " << currency << std::endl;
    return true;
}