#include <iostream>
#include <map>
#include <string>
#include "../third_party/httplib.h"
#include "../third_party/json.hpp"
#include <curl/curl.h>
#include <algorithm>
#include <cctype>

using json = nlohmann::json;

std::map<std::string, double> wallet;

double roundTo2Decimals(double value) {
    return std::round(value * 100.0) / 100.0;
}

// Callback function for libcurl to write data
static size_t WriteCallback(char* data, size_t size, size_t nmemb, std::string* response_data)
{
    if (response_data == nullptr) {
        return 0;
    }
    
    response_data->append(data, size * nmemb);
    return size * nmemb;
}

double fetchNBPRate(const std::string& currency) {
    std::string url = "https://api.nbp.pl/api/exchangerates/rates/a/" + currency + "/?format=json";
    std::string response_data;
    char errorBuffer[CURL_ERROR_SIZE];
    
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize CURL" << std::endl;
        return -1.0;
    }
    
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
        
    CURLcode result;
    result = curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    if(result != CURLE_OK) {
        std::cerr << "Failed to set URL: " << errorBuffer << std::endl;
        curl_easy_cleanup(curl);
        return -1.0;
    }

    result = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    if (result != CURLE_OK) {
        std::cerr << "Failed to set write function: " << errorBuffer << std::endl;
        curl_easy_cleanup(curl);
        return -1.0;
    }

    result = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
    if (result != CURLE_OK) {
        std::cerr << "Failed to set write data: " << errorBuffer << std::endl;
        curl_easy_cleanup(curl);
        return -1.0;
    }

    // Perform the request
    result = curl_easy_perform(curl);
    if (result != CURLE_OK) {
        std::cerr << "CURL request failed: " << errorBuffer << std::endl;
        curl_easy_cleanup(curl);
        return -1.0;
    }

    curl_easy_cleanup(curl);

    // Parse JSON response
    try {
        json nbp_response = json::parse(response_data);
        double rate = nbp_response["rates"][0]["mid"];
        
        std::cout << "Fetched rate for " << currency << ": " << roundTo2Decimals(rate) << " PLN" << std::endl;
        return rate;
        
    } catch (const json::exception& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return -1.0;
    }
}



int main() {
    std::cout << "Currency Wallet API" << std::endl;
    
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

        wallet[currency] += amount;

        json response;
        response["message"] = "Currency added";
        response["currency"] = currency;
        response["amount"] = roundTo2Decimals(amount);
        response["total"] = roundTo2Decimals(wallet[currency]);

        res.set_content(response.dump(2), "application/json");
    });

    // POST /wallet/sub endpoint
    srv.Post("/wallet/sub", [](const httplib::Request& req, httplib::Response& res) {
        std::cout << "POST /wallet/sub" << std::endl;

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
        if (wallet.find(currency) == wallet.end()) {
            res.status = 400;
            json error_response;
            error_response["error"] = "No such currency in wallet";
            error_response["currency"] = currency;
            res.set_content(error_response.dump(2), "application/json");
            return;
        }

        // Check if there is enough funds in wallet
        if (wallet[currency] < amount) {
            res.status = 400;
            json error_response;
            error_response["error"] = "Not enough funds";
            error_response["currency"] = currency;
            error_response["available"] = wallet[currency];
            error_response["requested"] = amount;
            res.set_content(error_response.dump(2), "application/json");
            return;
        }

        wallet[currency] -= amount;

        json response;
        response["message"] = "Currency subsracted";
        response["currency"] = currency;
        response["amount"] = roundTo2Decimals(amount);
        response["total"] = roundTo2Decimals(wallet[currency]);

        res.set_content(response.dump(2), "application/json");
    });

    // GET /wallet endpoint
    srv.Get("/wallet", [](const httplib::Request&, httplib::Response& res) {
        std::cout << "GET /wallet" << std::endl;
        
        json wallet_array = json::array();
        double total_pln = 0.0;

        for(auto& [currency, amount] : wallet) {
            double rate = fetchNBPRate(currency);
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
