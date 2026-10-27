#pragma once

#include "backend.hpp"

#include <cstdint>
#include <vector>

// Walks the sorted node list one entry at a time. O(N) worst case.
class LinearRing : public BackendRing {
public:
    using BackendRing::BackendRing;
    const Backend& lookup(uint32_t key, int& hops) override;
};

// Chord-style finger table: each node keeps pointers to the successors of
// n+2^0, n+2^1, ..., n+2^(M-1) (mod HASH_SPACE), so a lookup can skip
// large arcs of the ring instead of visiting every node. O(log N) hops.
class FingerRing : public BackendRing {
private:
    static constexpr int M = FINGER_BITS;

    std::vector<std::vector<int>> fingers; // fingers[node_index][i]

    int successor_index(uint32_t key) const;
    void build_fingers();

public:
    explicit FingerRing(std::vector<Backend> backends);

    const Backend& lookup(uint32_t key, int& hops) override;
    void print_fingers() const;
};
