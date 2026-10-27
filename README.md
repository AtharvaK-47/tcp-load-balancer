# TCP Load Balancer: Linear Ring vs. Chord Lookup

A C++ project that explores TCP load balancing and compares linear ring-based lookup with Chord-style finger-table lookup. The project includes client, server, load-balancer, and benchmark components to study lookup performance.

## Overview

In a distributed system, a load balancer helps direct incoming requests to available backend servers. How the system locates a destination can affect lookup cost as the number of nodes increases.

This project investigates two lookup approaches:

- **Linear ring lookup:** searches through ring nodes sequentially.
- **Chord-style finger-table lookup:** uses finger-table entries to navigate the ring more efficiently.

The benchmark component is intended to compare lookup performance, including the number of lookup hops.

## Project Structure

```text
tcp-load-balancer/
├── src/
│   ├── server.cpp
│   ├── client.cpp
│   ├── load_balancer.cpp
│   ├── benchmark.cpp
│   ├── hash.cpp
│   ├── hash.hpp
│   ├── ring.cpp
│   ├── ring.hpp
│   ├── backend.hpp
│   └── config.hpp
├── scripts/
│   └── run_demo.sh
├── results/
│   ├── benchmark_results.csv
│   └── benchmark_graph.png
├── docs/
├── Makefile
├── README.md
└── .gitignore
```

## Requirements

- Linux or WSL2 running Ubuntu
- A C++ compiler such as `g++`
- GNU Make
- POSIX socket support

On Ubuntu, install the basic build tools with:

```bash
sudo apt update
sudo apt install build-essential
```

## Build

From the repository root, run:

```bash
make
```

This should build the server, client, load balancer, and benchmark executables.

To remove generated executables:

```bash
make clean
```

## Running the Project

Start the components using the commands and port configuration implemented by the source code. For example, the intended components are:

1. Start the backend server or servers.
2. Start the load balancer.
3. Run the client to send requests through the load balancer.

The exact startup order, arguments, ports, and number of backend servers depend on the implementation in `src/` and `config.hpp`.

If the demo script is configured for your machine, run it from the repository root:

```bash
bash scripts/run_demo.sh
```

## Benchmarking

The benchmark component compares the lookup approaches under the configurations supported by the implementation.

Run the benchmark executable:

```bash
./bin/benchmark
```

The existing benchmark data and graph are stored in:

- `results/benchmark_results.csv`
- `results/benchmark_graph.png`

The CSV contains the recorded measurements, while the graph provides a visual comparison. Refer to the benchmark implementation for the precise metrics, workload, and experimental settings.

## Technologies

- C++
- TCP/IP sockets
- Ring-based lookup
- Chord-style finger tables
- GNU Make
- CSV-based benchmark results

## Limitations

Results depend on the implementation, workload, number of nodes, and experimental configuration. Lookup-hop counts alone do not establish end-to-end network latency or throughput.

## Future Improvements

- Test larger ring sizes and different request workloads.
- Compare lookup-hop counts and elapsed lookup time.
- Document the benchmark configuration and reproducibility steps.
- Add automated tests for routing correctness.

## Author

Add your name and course or project details here, if appropriate.
