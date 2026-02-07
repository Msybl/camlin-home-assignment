#include <iostream>
#include <map>
#include <string>
#include "../third_party/httplib.h"
#include "../third_party/json.hpp"

using json = nlohmann::json;

std::map<std::string, double> wallet;

int main() {
    std::cout << "Currency Wallet API" << std::endl;
    
    httplib::Server srv;
    
    // GET /health endpoint
    srv.Get("/health", [](const httplib::Request &, httplib::Response &res) {
        res.set_content("{\"status\":\"ok\",\"message\":\"Currency Wallet API\"}", "application/json");
    });

    // GET / endpoint
    srv.Get("/", [](const httplib::Request &, httplib::Response &res) {
        res.set_content("{\"status\":\"ok\",\"message\":\"Currency Wallet API\"}", "application/json");
    });
    
    // 404 handler
    srv.set_error_handler([](const httplib::Request &, httplib::Response &res) {
        res.set_content("{\"error\":\"Not Found\"}", "application/json");
    });

    // POST /wallet/add endpoint
    srv.Post("/wallet/add", [](const httplib::Request &req, httplib::Response & res) {
        std::cout << "POST /wallet/add" << std::endl;

        json req_data = json::parse(req.body);
        
        // TODO: check parsing errors
        // TODO: check if the fields exist

        std::string currency = req_data["currency"];
        double amount = req_data["amount"];

        // TODO: check if amount is positive

        wallet[currency] += amount;

        json response;
        response["message"] = "Currency added";
        response["currency"] = currency;
        response["amount"] = amount;
        response["total"] = wallet[currency];

        res.set_content(response.dump(2), "application/json");
    });

    // POST /wallet/sub endpoint
    srv.Post("/wallet/sub", [](const httplib::Request &req, httplib::Response & res) {
        std::cout << "POST /wallet/sub" << std::endl;

        json req_data = json::parse(req.body);
        
        // TODO: check parsing errors
        // TODO: check if the fields exist

        std::string currency = req_data["currency"];
        double amount = req_data["amount"];

        // TODO: check if amount is positive

        wallet[currency] -= amount;

        json response;
        response["message"] = "Currency subsracted";
        response["currency"] = currency;
        response["amount"] = amount;
        response["total"] = wallet[currency];

        res.set_content(response.dump(2), "application/json");
    });

    // GET /wallet endpoint
    srv.Get("/wallet", [](const httplib::Request &, httplib::Response &res) {
        std::cout << "GET /wallet" << std::endl;
        
        json wallet_array = json::array();

        for(auto &[currency, amount] : wallet) {
            json item;
            item["currency"] = currency;
            item["amount"] = amount;
            wallet_array.push_back(item);
        }

        json response;
        response["wallet"] = wallet_array;
        res.set_content(response.dump(2), "application/json");
    });

    
    // Start server
    std::cout << "Server listening on port 8080" << std::endl;
    srv.listen("0.0.0.0", 8080);
    
    return 0;
}
