#include <iostream>
#include "../third_party/httplib.h"

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
    
    // Start server
    std::cout << "Server listening on port 8080" << std::endl;
    srv.listen("0.0.0.0", 8080);
    
    return 0;
}
