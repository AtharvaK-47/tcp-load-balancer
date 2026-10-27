#pragma once

#include "config.hpp"

#include <algorithm>
#include <cstdint>
#include <vector>

// Common storage + interface for any routing strategy placed on the ring.
class BackendRing {
protected:
    std::vector<Backend> nodes;

public:
    explicit BackendRing(std::vector<Backend> backends)
        : nodes(std::move(backends)) {
        std::sort(nodes.begin(), nodes.end(),
                  [](const Backend& a, const Backend& b) { return a.id < b.id; });
    }

    virtual ~BackendRing() = default;

    // Resolves `key` to the backend responsible for it and reports how
    // many ring hops the lookup took, so strategies can be compared.
    virtual const Backend& lookup(uint32_t key, int& hops) = 0;

    const std::vector<Backend>& get_nodes() const { return nodes; }
};
