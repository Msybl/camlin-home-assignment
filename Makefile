CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
LDFLAGS = -lpthread
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
	@echo "Testing endpoint..."
	@curl -s http://localhost:8080/hi || true
	@echo ""
	@echo "Stopping server..."
	@kill `cat .server.pid` 2>/dev/null || true
	@rm -f .server.pid

.PHONY: all clean run test
