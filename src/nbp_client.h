#ifndef NBP_CLIENT_H
#define NBP_CLIENT_H

#include <string>
#include "../third_party/json.hpp"

using json = nlohmann::json;

// Fetch exchange rate from NBP API
double fetchNBPRate(const std::string& currency);

// Fetch Table C which contains "Ask" rates
std::map<std::string, double> fetchAllNBPRates();

#endif // NBP_CLIENT_H