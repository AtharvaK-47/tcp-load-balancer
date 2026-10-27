#include "config.hpp"
#include "hash.hpp"
#include "ring.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <memory>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <string>

int connect_backend(const Backend& backend) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(backend.port);
    inet_pton(AF_INET, backend.host.c_str(), &addr.sin_addr);

    if (connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        close(fd);
        return -1;
    }
    return fd;
}

std::string extract_value(const std::string& request, const std::string& key) {
    std::string prefix = key + "=";
    size_t pos = request.find(prefix);
    if (pos == std::string::npos) return "";
    pos += prefix.size();
    size_t end = request.find('\n', pos);
    if (end == std::string::npos) end = request.size();
    return request.substr(pos, end - pos);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: ./lb <linear|finger>\n";
        return 1;
    }

    std::string mode = argv[1];
    std::unique_ptr<BackendRing> ring;

    if (mode == "linear")      ring = std::make_unique<LinearRing>(BACKENDS);
    else if (mode == "finger") ring = std::make_unique<FingerRing>(BACKENDS);
    else { std::cerr << "Unknown mode: " << mode << "\n"; return 1; }

    std::cout << "Load Balancer\nMode: " << mode << "\nListening on " << LB_PORT << "\n\n";

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket"); return 1; }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(LB_PORT);

    if (bind(server_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        perror("bind"); return 1;
    }
    if (listen(server_fd, 50) < 0) { perror("listen"); return 1; }

    while (true) {
        sockaddr_in client{};
        socklen_t len = sizeof(client);
        int client_fd = accept(server_fd, reinterpret_cast<sockaddr*>(&client), &len);
        if (client_fd < 0) { perror("accept"); continue; }

        char buffer[4096]{};
        ssize_t n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) { close(client_fd); continue; }

        std::string request(buffer, n);
        int client_id = std::stoi(extract_value(request, "CLIENT_ID"));
        int vlan      = std::stoi(extract_value(request, "VLAN"));

        uint32_t key = hash_client(client_id, vlan) % HASH_SPACE;

        int hops = 0;
        const Backend& backend = ring->lookup(key, hops);

        std::cout << "[LB] client=" << client_id << " vlan=" << vlan
                   << " hash=" << key << " backend=" << backend.port
                   << " hops=" << hops << std::endl;

        int backend_fd = connect_backend(backend);
        if (backend_fd < 0) {
            std::string error = "Backend unavailable\n";
            send(client_fd, error.data(), error.size(), 0);
            close(client_fd);
            continue;
        }

        send(backend_fd, request.data(), request.size(), 0);

        char response[4096]{};
        ssize_t response_size = recv(backend_fd, response, sizeof(response) - 1, 0);
        if (response_size > 0) send(client_fd, response, response_size, 0);

        close(backend_fd);
        close(client_fd);
    }
}
