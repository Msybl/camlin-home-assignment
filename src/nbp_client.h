#ifndef NBP_CLIENT_H
#define NBP_CLIENT_H

#include <string>
#include "../third_party/json.hpp"

using json = nlohmann::json;

// Fetch exchange rate from NBP API
double fetchNBPRate(const std::string& currency);

#endif // NBP_CLIENT_H