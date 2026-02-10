# Currency Wallet API
A REST API for tracking the current Polish złoty (PLN) value of money held in foreign currencies. It uses realtime exchange rates from the National Bank of Poland (NBP). Built with C++. 
- Build: Make
- Web framework: cpp-httplib
- JSON: nlohmann/json
- HTTP Client: libcurl (for NBP rates)
- Database: SQLite3

## Prerequisites
### Running with Docker
- Docker (v20+)
- Docker Compose (v2+)

### Running Locally
- Linux or WSL (Windows Subsystem for Linux)
- g++ with C++17 support
- make
- libcurl (`sudo apt install libcurl4-openssl-dev`)
- libsqlite3 (`sudo apt install libsqlite3-dev`)

## Quick Start
### Docker
```bash
# Clone the repository
git clone <repository-url>
cd camlin-home-assignment

# Build and start
docker-compose up -d

# Test the health API
curl http://localhost:8080/health

# View logs
docker-compose logs -f

# Stop
docker-compose down
```

### Local
```bash
# Build
make

# Clean build
make clean && make

# Run
./wallet_api

# Run tests
make test
```

## Authentication

All endpoints require an API key passed in the `X-API-Key` header:

### Valid API Keys
| API Key | User |
|---------|------|
| `key-123` | user1 |
| `key-456` | user2 |
| `key-789` | user3 |

### Example
```bash
curl -H "X-API-Key: key-123" http://localhost:8080/wallet
```
## API Endpoints

### Health Check

```
GET /health
```

No authentication required

**Response:**
```json
{
  "status": "ok",
  "message": "Currency Wallet API"
}
```

---

### Get Wallet

```
GET /wallet
```

Returns the current wallet composition with realtime PLN values for each currency

**Headers:**
```
X-API-Key: key-123
```

**Response:**
```json
{
  "wallet": [
    {
      "currency": "EUR",
      "amount": 100.0,
      "rate": 4.26,
      "pln_value": 426.0
    },
    {
      "currency": "USD",
      "amount": 50.0,
      "rate": 3.61,
      "pln_value": 180.5
    }
  ],
  "total_pln": 606.5
}
```

---

### Add

```
POST /wallet/add
```

Adds an amount of a currency to the wallet. Creates the currency if it doesn't exist

**Headers:**
```
X-API-Key: key-123
Content-Type: application/json
```

**Request Body:**
```json
{
  "currency": "USD",
  "amount": 100.0
}
```

**Response:**
```json
{
  "message": "Currency added",
  "currency": "USD",
  "amount": 100.0,
  "total": 100.0
}
```

---

### Sub

```
POST /wallet/sub
```

Subtracts an amount of a currency from the wallet. Removes the currency if balance becomes zero

**Headers:**
```
X-API-Key: key-123
Content-Type: application/json
```

**Request Body:**
```json
{
  "currency": "USD",
  "amount": 30.0
}
```

**Response:**
```json
{
  "message": "Currency subtracted",
  "currency": "USD",
  "amount": 30.0,
  "total": 70.0
}
```
## Error Handling
| Code | Meaning |
|------|---------|
| 400 | Bad Request (invalid input, insufficient funds) |
| 401 | Unauthorized (missing/invalid API key) |
| 404 | Not Found |
| 500 | Internal Server Error (NBP API unavailable) |

## Example Usage
```bash
# Add EUR to wallet
curl -s -X POST http://localhost:8080/wallet/add \
    -H "X-API-Key: key-123" \
    -H "Content-Type: application/json" \
    -d '{"currency":"EUR","amount":75}'

# Add USD to wallet
curl -s -X POST http://localhost:8080/wallet/add \
    -H "X-API-Key: key-123" \
    -H "Content-Type: application/json" \
    -d '{"currency":"USD","amount":100}'

# Subtract from wallet
curl -s -X POST http://localhost:8080/wallet/sub \
    -H "X-API-Key: key-123" \
    -H "Content-Type: application/json" \
    -d '{"currency":"USD","amount":30}'

# Get wallet PLN values (with NBP rates)
curl -s -H "X-API-Key: key-123" http://localhost:8080/wallet
```

## Decisions & Notes
- cpp-httplib has been chosen as the web framework. I know its blocking I/O creates one thread per request which means scalibility issue. However, I made a pragmatic decision and prioritized fast development. For production, I saw more suitable frameworks such as Drogon
- Using double for simplicity. For production, a decimal library like boost::multiprecision can be used
- Decided to remove currency if the balance is 0 due to unnecessary logs regarding empty currencies
- Hardcoded valid API keys are used in auth.cpp (in real production code, these should be in a secure config)
