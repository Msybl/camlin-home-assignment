#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <map>

// Initialize database and create wallet table
bool initDatabase();

// Load wallet from database
bool loadWalletFromDB(std::map<std::string, double>& wallet);

#endif // DATABASE_H