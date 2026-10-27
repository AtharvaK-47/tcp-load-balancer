#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct Backend {
    std::string host;
    int port;
    uint32_t id;      // position on the hash ring
    bool alive = true;
};

struct ClientInfo {
    int id;
    int vlan;
};

inline constexpr int LB_PORT = 9000;

// Deployment backend set used by the live load balancer (server.cpp / client.cpp demo).
inline const std::vector<Backend> BACKENDS = {
    {"127.0.0.1", 8001, 7500},
    {"127.0.0.1", 8002, 25000},
    {"127.0.0.1", 8003, 42500},
    {"127.0.0.1", 8004, 58500}
};

// 2^16 identifier space -> M = 16 finger entries per node.
inline constexpr uint32_t HASH_SPACE = 65536;
inline constexpr int FINGER_BITS = 16;
