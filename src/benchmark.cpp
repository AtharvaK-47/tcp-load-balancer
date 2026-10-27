// benchmark.cpp
// Measures average / worst-case ring hops for LinearRing vs FingerRing
// as the number of backend nodes N grows, using random (client_id, VLAN)
// lookup keys. Produces benchmark_results.csv.

#include "hash.hpp"
#include "ring.hpp"
#include "config.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

static std::vector<Backend> make_ring_nodes(int n) {
    std::vector<Backend> nodes;
    nodes.reserve(n);
    for (int i = 0; i < n; ++i) {
        Backend b;
        b.host = "127.0.0.1";
        b.port = 9000 + i;
        // Evenly spaced identifiers around the ring.
        b.id = static_cast<uint32_t>((static_cast<uint64_t>(i) * HASH_SPACE) / n);
        b.alive = true;
        nodes.push_back(b);
    }
    return nodes;
}

int main() {
    const std::vector<int> Ns = {4, 8, 16, 32, 64, 128, 256};
    const int TRIALS = 5000;

    std::mt19937 rng(42); // fixed seed -> reproducible results
    std::uniform_int_distribution<int> client_dist(1, 1000000);
    std::uniform_int_distribution<int> vlan_dist(10, 30);

    std::ofstream csv("benchmark_results.csv");
    csv << "N,avg_linear_hops,max_linear_hops,avg_finger_hops,max_finger_hops\n";

    std::cout << "N\tavg_linear\tmax_linear\tavg_finger\tmax_finger\n";

    for (int n : Ns) {
        auto nodes = make_ring_nodes(n);

        LinearRing linear(nodes);
        FingerRing finger(nodes);

        long long sum_linear = 0, sum_finger = 0;
        int max_linear = 0, max_finger = 0;

        for (int t = 0; t < TRIALS; ++t) {
            int cid = client_dist(rng);
            int vlan = vlan_dist(rng);
            uint32_t key = hash_client(cid, vlan) % HASH_SPACE;

            int hops_l = 0, hops_f = 0;
            linear.lookup(key, hops_l);
            finger.lookup(key, hops_f);

            sum_linear += hops_l;
            sum_finger += hops_f;
            max_linear = std::max(max_linear, hops_l);
            max_finger = std::max(max_finger, hops_f);
        }

        double avg_linear = static_cast<double>(sum_linear) / TRIALS;
        double avg_finger = static_cast<double>(sum_finger) / TRIALS;

        std::cout << n << "\t" << avg_linear << "\t\t" << max_linear
                   << "\t\t" << avg_finger << "\t\t" << max_finger << "\n";

        csv << n << "," << avg_linear << "," << max_linear << ","
            << avg_finger << "," << max_finger << "\n";
    }

    std::cout << "\nWrote benchmark_results.csv\n";
    return 0;
}
