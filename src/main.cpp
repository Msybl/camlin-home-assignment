#include <iostream>
#include <map>
#include <string>
#include "../third_party/httplib.h"
#include "../third_party/json.hpp"
#include <algorithm>
#include <cctype>
#include "nbp_client.h"
#include "database.h"
#include "utils.h"
#include "auth.h"

// Wallet for each user
std::map<std::string, std::map<std::string, double>> user_wallets;

int main() {
    std::cout << "Currency Wallet API" << std::endl;

    // Initialize database
    if (!initDatabase()) {
        std::cerr << "Failed to initialize database" << std::endl;
        return 1;
    }

    httplib::Server srv;
    
    // GET /health endpoint
    srv.Get("/health", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("{\"status\":\"ok\",\"message\":\"Currency Wallet API\"}", "application/json");
    });

    // GET / endpoint
    srv.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("{\"status\":\"ok\",\"message\":\"Currency Wallet API\"}", "application/json");
    });
    
    // POST /wallet/add endpoint
    srv.Post("/wallet/add", [](const httplib::Request& req, httplib::Response& res) {
        std::cout << "POST /wallet/add" << std::endl;

        // Authenticate request
        std::string user_id = authenticateRequest(req, res);
        if (user_id.empty()) {
            return;  
        }

        // Load userss wallet if it doesn't exist
        if (user_wallets.find(user_id) == user_wallets.end()) {
            user_wallets[user_id] = std::map<std::string, double>();
            loadWalletFromDB(user_id, user_wallets[user_id]);
        }

        json req_data;
        
        // Parse JSON with error handling
        try {
            req_data = json::parse(req.body);
        } catch(const json::parse_error& e) {
            res.status = 400;
            json error_response;
            error_response["error"] = "Invalid JSON";
            error_response["details"] = e.what();
            res.set_content(error_response.dump(2), "application/json");
            return;
        }

        // Check if the fields exist
        if(!req_data.contains("currency") || !req_data.contains("amount")) {
            res.status = 400;
            json error_response;
            error_response["error"] = "Missing required fields";
            error_response["required"] = {"currency", "amount"};
            res.set_content(error_response.dump(2), "application/json");
            return;
        }

        std::string currency = req_data["currency"];
        double amount = req_data["amount"];

        // Check if amount is bigger than 0
        if(amount <= 0) {
            res.status = 400;
            json error_response;
            error_response["error"] = "Amount must be bigger than 0";
            error_response["received"] = amount;
            res.set_content(error_response.dump(2), "application/json");
            return;
        }

        // Check 3-letter currency code (ISO 4217 standard)
        if (currency.length() != 3) {
            res.status = 400;
            json error_response;
            error_response["error"] = "Currency code must be 3 characters";
            error_response["received"] = currency;
            res.set_content(error_response.dump(2), "application/json");
            return;
        }

        // In case it is not uppercase, convert
        std::transform(currency.begin(), currency.end(), currency.begin(), ::toupper);

        user_wallets[user_id][currency] += amount;

        // Save to database
        if (!saveCurrencyToDB(user_id, currency, user_wallets[user_id][currency])) {
            std::cerr << "Warning: Failed to save to database" << std::endl;
        }

        json response;
        response["message"] = "Currency added";
        response["currency"] = currency;
        response["amount"] = roundTo2Decimals(amount);
        response["total"] = roundTo2Decimals(user_wallets[user_id][currency]);

        res.set_content(response.dump(2), "application/json");
    });

    // POST /wallet/sub endpoint
    srv.Post("/wallet/sub", [](const httplib::Request& req, httplib::Response& res) {
        std::cout << "POST /wallet/sub" << std::endl;

        // Authenticate request
        std::string user_id = authenticateRequest(req, res);
        if (user_id.empty()) {
            return;  
        }

        // Load userss wallet if it doesn't exist
        if (user_wallets.find(user_id) == user_wallets.end()) {
            user_wallets[user_id] = std::map<std::string, double>();
            loadWalletFromDB(user_id, user_wallets[user_id]);
        }

        json req_data;
        
        // Parse JSON with error handling
        try{
            req_data = json::parse(req.body);
        } catch(const json::parse_error& e) {
            res.status = 400;
            json error_response;
            error_response["error"] = "Invalid JSON";
            error_response["details"] = e.what();
            res.set_content(error_response.dump(2), "application/json");
            return;
        }
        
        // Check if the fields exist
        if(!req_data.contains("currency") || !req_data.contains("amount")) {
            res.status = 400;
            json error_response;
            error_response["error"] = "Missing required fields";
            error_response["required"] = {"currency", "amount"};
            res.set_content(error_response.dump(2), "application/json");
            return;
        }

        std::string currency = req_data["currency"];
        double amount = req_data["amount"];

        // Check if amount is bigger than 0
        if(amount <= 0) {
            res.status = 400;
            json error_response;
            error_response["error"] = "Amount must be bigger than 0";
            error_response["received"] = amount;
            res.set_content(error_response.dump(2), "application/json");
            return;
        }

        // Check 3-letter currency code (ISO 4217 standard)
        if (currency.length() != 3) {
            res.status = 400;
            json error_response;
            error_response["error"] = "Currency code must be 3 characters";
            error_response["received"] = currency;
            res.set_content(error_response.dump(2), "application/json");
            return;
        }

        // In case it is not uppercase, convert
        std::transform(currency.begin(), currency.end(), currency.begin(), ::toupper);

        // Check if there is the reuested currency in wallet
        if (user_wallets[user_id].find(currency) == user_wallets[user_id].end()) {
            res.status = 400;
            json error_response;
            error_response["error"] = "No such currency in wallet";
            error_response["currency"] = currency;
            res.set_content(error_response.dump(2), "application/json");
            return;
        }

        // Check if there is enough funds in wallet
        if (user_wallets[user_id][currency] < amount) {
            res.status = 400;
            json error_response;
            error_response["error"] = "Not enough funds";
            error_response["currency"] = currency;
            error_response["available"] = user_wallets[user_id][currency];
            error_response["requested"] = amount;
            res.set_content(error_response.dump(2), "application/json");
            return;
        }

        user_wallets[user_id][currency] -= amount;

        // Save the new amount
        double new_amount = user_wallets[user_id][currency];

        // Delete if zero (or close to zero due to double type amount)
        if (new_amount <= 0.01) {
            user_wallets[user_id].erase(currency);
            if (!deleteCurrencyFromDB(user_id, currency)) {
                std::cerr << "Failed to delete from database" << std::endl;
            }
        } else {
            // Save updated amount
            if (!saveCurrencyToDB(user_id, currency, new_amount)) {
                std::cerr << "Failed to save to database" << std::endl;
            }
        }

        json response;
        response["message"] = "Currency subsracted";
        response["currency"] = currency;
        response["amount"] = roundTo2Decimals(amount);
        response["total"] = roundTo2Decimals(new_amount);

        res.set_content(response.dump(2), "application/json");
    });

    // GET /wallet endpoint
    srv.Get("/wallet", [](const httplib::Request& req, httplib::Response& res) {
        std::cout << "GET /wallet" << std::endl;

        // Authenticate request
        std::string user_id = authenticateRequest(req, res);
        if (user_id.empty()) {
            return;  
        }

        // Load userss wallet if it doesn't exist
        if (user_wallets.find(user_id) == user_wallets.end()) {
            user_wallets[user_id] = std::map<std::string, double>();
            loadWalletFromDB(user_id, user_wallets[user_id]);
        }

        // Fetch all NBP Table C rates at once
        std::map<std::string, double> nbp_rates = fetchAllNBPRates();
        if (nbp_rates.empty()) {
            res.status = 500;
            json error_response;
            error_response["error"] = "Failed to fetch exchange rates from NBP";
            res.set_content(error_response.dump(2), "application/json");
            return;
        }
        
        json wallet_array = json::array();
        double total_pln = 0.0;

        for(auto& [currency, amount] : user_wallets[user_id]) {
            // Check if rate exists for this currency
            if (nbp_rates.find(currency) == nbp_rates.end()) {
                std::cerr << "No NBP rate found: " << currency << std::endl;
                // Skip currencies that are not in NBP Table C
                continue;  
            }

            double rate = nbp_rates[currency];
            double pln_value = amount * rate;
            total_pln += pln_value;
            
            json item;
            item["currency"] = currency;
            item["amount"] = roundTo2Decimals(amount);
            item["rate"] = roundTo2Decimals(rate);
            item["pln_value"] = roundTo2Decimals(pln_value);
            
            wallet_array.push_back(item);
        }

        json response;
        response["wallet"] = wallet_array;
        response["total_pln"] = roundTo2Decimals(total_pln);
        res.set_content(response.dump(2), "application/json");
    });

    
    // Start server
    std::cout << "Server listening on port 8080" << std::endl;
    srv.listen("0.0.0.0", 8080);
    
    return 0;
}
