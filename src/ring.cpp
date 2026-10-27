#include "ring.hpp"
#include "config.hpp"

#include <iostream>

// ---------------------------------------------------------------------
// Linear ring: scan sorted nodes until one with id >= key is found.
// ---------------------------------------------------------------------

const Backend& LinearRing::lookup(uint32_t key, int& hops) {
    hops = 0;
    for (size_t i = 0; i < nodes.size(); ++i) {
        ++hops;
        if (key <= nodes[i].id)
            return nodes[i];
    }
    return nodes[0]; // wrap around the ring
}

// ---------------------------------------------------------------------
// Finger-table ring
// ---------------------------------------------------------------------

FingerRing::FingerRing(std::vector<Backend> backends)
    : BackendRing(std::move(backends)) {
    build_fingers();
}

// First node whose id is >= key; defaults to node 0 (ring wraps).
int FingerRing::successor_index(uint32_t key) const {
    for (size_t i = 0; i < nodes.size(); ++i) {
        if (key <= nodes[i].id)
            return static_cast<int>(i);
    }
    return 0;
}

void FingerRing::build_fingers() {
    fingers.assign(nodes.size(), std::vector<int>(M));
    for (size_t i = 0; i < nodes.size(); ++i) {
        for (int j = 0; j < M; ++j) {
            uint32_t start = (nodes[i].id + (1u << j)) % HASH_SPACE;
            fingers[i][j] = successor_index(start);
        }
    }
}

// x strictly inside the cyclic open interval (lo, hi)
static bool cyclic_between_open(uint32_t x, uint32_t lo, uint32_t hi) {
    if (lo < hi) return x > lo && x < hi;
    return x > lo || x < hi; // interval wraps past 0
}

// x inside the cyclic half-open interval (lo, hi]
static bool cyclic_between_half_open(uint32_t x, uint32_t lo, uint32_t hi) {
    if (lo < hi) return x > lo && x <= hi;
    return x > lo || x <= hi;
}

// Standard Chord find_successor(): jump via the largest finger that does
// not overshoot the key, falling back to the immediate ring successor.
const Backend& FingerRing::lookup(uint32_t key, int& hops) {
    hops = 0;
    int current = 0; // arbitrary, fixed entry point into the ring

    while (true) {
        ++hops;

        const Backend& cur = nodes[current];
        int succ_idx = static_cast<int>((current + 1) % nodes.size());
        uint32_t succ_id = nodes[succ_idx].id;

        if (nodes.size() == 1 || cyclic_between_half_open(key, cur.id, succ_id))
            return nodes[succ_idx];

        int next = current;
        for (int i = M - 1; i >= 0; --i) {
            int candidate = fingers[current][i];
            uint32_t cand_id = nodes[candidate].id;
            if (cyclic_between_open(cand_id, cur.id, key)) {
                next = candidate;
                break;
            }
        }

        current = (next == current) ? succ_idx : next;

        // Safety net against a malformed/degenerate ring.
        if (hops > static_cast<int>(nodes.size()) + M + 2)
            return nodes[current];
    }
}

void FingerRing::print_fingers() const {
    std::cout << "\nFinger Tables\n-------------\n";
    for (size_t i = 0; i < nodes.size(); ++i) {
        std::cout << "Node " << nodes[i].id << ": ";
        for (int j = 0; j < M; ++j) {
            std::cout << fingers[i][j];
            if (j != M - 1) std::cout << " ";
        }
        std::cout << "\n";
    }
}
