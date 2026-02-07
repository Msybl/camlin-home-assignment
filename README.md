# Camlin-home-assignement
- Web framework: cpp-httplib
- JSON: nlohmann/json

# Notes
- cpp-httplib has been chosen as the web framework. I know its blocking I/O creates one thread per request which means scalibility issue. However, I made a pragmatic decision and prioritized fast development. For production, I saw more suitable frameworks such as Drogon
- Using double for simplicity. For production, a decimal library like boost::multiprecision can be used
