
CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -pthread
SRC_DIR := src
BIN_DIR := bin

COMMON_SRC := $(SRC_DIR)/hash.cpp $(SRC_DIR)/ring.cpp

.PHONY: all server client lb benchmark clean

all: server client lb benchmark

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

server: $(BIN_DIR)/server

$(BIN_DIR)/server: $(SRC_DIR)/server.cpp $(COMMON_SRC) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

client: $(BIN_DIR)/client

$(BIN_DIR)/client: $(SRC_DIR)/client.cpp $(COMMON_SRC) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

lb: $(BIN_DIR)/load_balancer

$(BIN_DIR)/load_balancer: $(SRC_DIR)/load_balancer.cpp $(COMMON_SRC) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

benchmark: $(BIN_DIR)/benchmark

$(BIN_DIR)/benchmark: $(SRC_DIR)/benchmark.cpp $(COMMON_SRC) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -rf $(BIN_DIR)
