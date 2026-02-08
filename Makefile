CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
LDFLAGS = -lpthread -lcurl -lsqlite3
TARGET = wallet_api
SRC_DIR = src
SOURCES = $(SRC_DIR)/main.cpp $(SRC_DIR)/nbp_client.cpp $(SRC_DIR)/database.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

test: $(TARGET)
	@echo "========================================="
	@echo "  Currency Wallet API Tests"
	@echo "========================================="
	@echo ""
	
	# Start server
	@echo "Starting server..."
	@./$(TARGET) & echo $$! > .server.pid
	@sleep 1
	@echo ""
	
	# Test health endpoint
	@echo "GET /health"
	@curl -s http://localhost:8080/health || true
	@echo ""
	@echo ""

	# Test root endpoint
	@echo "GET /"
	@curl -s http://localhost:8080/ || true
	@echo ""
	@echo ""
	
	@echo "========================================="
	@echo "  WALLET TESTS"
	@echo "========================================="
	@echo ""
	
	# Add EUR to wallet
	@echo "POST /wallet/add 75 EUR"
	@curl -s -X POST http://localhost:8080/wallet/add \
		-H "Content-Type: application/json" \
		-d '{"currency":"EUR","amount":75}' || true
	@echo ""
	@echo ""

	# Add USD to wallet
	@echo "POST /wallet/add 100 USD"
	@curl -s -X POST http://localhost:8080/wallet/add \
		-H "Content-Type: application/json" \
		-d '{"currency":"USD","amount":100}' || true
	@echo ""
	@echo ""

	# Subtract from wallet
	@echo "POST /wallet/sub 30 USD"
	@curl -s -X POST http://localhost:8080/wallet/sub \
		-H "Content-Type: application/json" \
		-d '{"currency":"USD","amount":30}' || true
	@echo ""
	@echo ""
	
	# Get wallet PLN values (with NBP rates)
	@echo "GET /wallet"
	@curl -s http://localhost:8080/wallet || true
	@echo ""
	@echo ""

	@echo "========================================="
	@echo "  WALLET VALIDATION TESTS (ERROR CASES)"
	@echo "========================================="
	@echo ""

	# Invalid JSON 
	@echo "Invalid JSON (400)"
	@curl -s -X POST http://localhost:8080/wallet/add \
		-H "Content-Type: application/json" \
		-d '{invalid json}' || true
	@echo ""
	@echo ""

	# Missing field
	@echo "Missing 'amount' field (400)"
	@curl -s -X POST http://localhost:8080/wallet/add \
		-H "Content-Type: application/json" \
		-d '{"currency":"USD"}' || true
	@echo ""
	@echo ""
	
	# Negative amount
	@echo "Negative amount (400)"
	@curl -s -X POST http://localhost:8080/wallet/add \
		-H "Content-Type: application/json" \
		-d '{"currency":"USD","amount":-100}' || true
	@echo ""
	@echo ""

	# Zero amount
	@echo "Zero amount (400)"
	@curl -s -X POST http://localhost:8080/wallet/add \
		-H "Content-Type: application/json" \
		-d '{"currency":"USD","amount":0}' || true
	@echo ""
	@echo ""

	# Short currency code
	@echo "Short currency code (400)"
	@curl -s -X POST http://localhost:8080/wallet/add \
		-H "Content-Type: application/json" \
		-d '{"currency":"US","amount":100}' || true
	@echo ""
	@echo ""
	
	# Long currency code
	@echo "Long currency code (400)"
	@curl -s -X POST http://localhost:8080/wallet/add \
		-H "Content-Type: application/json" \
		-d '{"currency":"USDD","amount":100}' || true
	@echo ""
	@echo ""

	# Lowercase currency code
	@echo "Lowercase currency code (should be converted to uppercase)"
	@curl -s -X POST http://localhost:8080/wallet/add \
		-H "Content-Type: application/json" \
		-d '{"currency":"gbp","amount":50}' || true
	@echo ""
	@echo ""

	# Not enough funds
	@echo "Not enough funds (400)"
	@curl -s -X POST http://localhost:8080/wallet/sub \
		-H "Content-Type: application/json" \
		-d '{"currency":"USD","amount":999999}' || true
	@echo ""
	@echo ""

	@echo "All tests completed!"
	@echo "Stopping server..."
	@kill `cat .server.pid` 2>/dev/null || true
	@rm -f .server.pid

.PHONY: all clean run test
