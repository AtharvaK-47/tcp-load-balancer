#pragma once

#include <cstdint>
#include <string>

uint32_t hash_string(const std::string& s);
uint32_t hash_client(int client_id, int vlan);
