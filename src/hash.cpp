#include "hash.hpp"

// FNV-1a: small, deterministic, well-distributed enough for a
// routing demonstration (not a cryptographic hash).
uint32_t hash_string(const std::string& s) {
    uint32_t h = 2166136261u;
    for (unsigned char c : s) {
        h ^= c;
        h *= 16777619u;
    }
    return h;
}

// VLAN is folded into the routing key on purpose:
//   client 7 + VLAN 10 -> hash("7:10")
//   client 7 + VLAN 20 -> hash("7:20")
// so the same client id can land on a different backend depending on
// which VLAN it is sourced from, without the C++ layer touching 802.1Q.
uint32_t hash_client(int client_id, int vlan) {
    return hash_string(std::to_string(client_id) + ":" + std::to_string(vlan));
}
