# Camlin-home-assignement
- Build: Make
- Web framework: cpp-httplib
- JSON: nlohmann/json
- HTTP Client: libcurl (for NBP rates)
- Database: SQLite3

# Notes
- cpp-httplib has been chosen as the web framework. I know its blocking I/O creates one thread per request which means scalibility issue. However, I made a pragmatic decision and prioritized fast development. For production, I saw more suitable frameworks such as Drogon
- Using double for simplicity. For production, a decimal library like boost::multiprecision can be used
- Decided to remove currency if the balance is 0 due to unnecessary logs regarding empty currencies
- Hardcoded valid API keys are used in main.cpp (in real production code, these should be in a secure config)
