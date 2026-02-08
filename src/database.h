#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <map>

// Initialize database and create wallet table
bool initDatabase();

// Load wallet from database
bool loadWalletFromDB(const std::string& user_id, std::map<std::string, double>& wallet);

// Save a currency to database
bool saveCurrencyToDB(const std::string& user_id, const std::string& currency, double amount);

// Delete a currency from database
bool deleteCurrencyFromDB(const std::string& user_id, const std::string& currency);

void testDatabaseOperations();

#endif // DATABASE_H