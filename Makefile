CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
LDFLAGS = -lpthread -lcurl
TARGET = wallet_api
SRC_DIR = src
SOURCES = $(SRC_DIR)/main.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

test: $(TARGET)
	@echo "Starting server..."
	@./$(TARGET) & echo $$! > .server.pid
	@sleep 1
	@echo "Testing endpoints..."
	@curl -s http://localhost:8080/health || true
	@echo ""
	@curl -s http://localhost:8080/ || true
	@echo ""
	@echo "GET /nonexistent (404):"
	@curl -s http://localhost:8080/nonexistent || true
	@echo ""
	@curl -X POST http://localhost:8080/wallet/add \
	-H "Content-Type: application/json" \
	-d '{"currency":"EUR","amount":75}'
	@echo ""
	@curl -X POST http://localhost:8080/wallet/add \
	-H "Content-Type: application/json" \
	-d '{"currency":"USD","amount":100}'
	@echo ""
	@curl -X POST http://localhost:8080/wallet/sub \
	-H "Content-Type: application/json" \
	-d '{"currency":"USD","amount":30}'
	@echo ""
	@curl -s http://localhost:8080/wallet || true
	@echo ""
	@echo "Stopping server..."
	@kill `cat .server.pid` 2>/dev/null || true
	@rm -f .server.pid

.PHONY: all clean run test
