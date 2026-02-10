# Stage 1: Build
FROM alpine:3.19 AS builder

RUN apk add --no-cache \
    build-base \
    sqlite-dev \
    curl-dev

WORKDIR /app

COPY src/ ./src/
COPY third_party/ ./third_party/
COPY Makefile .

RUN make

# Stage 2: Runtime
FROM alpine:3.19

RUN apk add --no-cache \
    libstdc++ \
    sqlite-libs \
    libcurl

WORKDIR /app

# Data directory
RUN mkdir -p /app/data

COPY --from=builder /app/wallet_api .

EXPOSE 8080

CMD ["./wallet_api"]