#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <string>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: ./server <port>\n";
        return 1;
    }

    int port = std::stoi(argv[1]);
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket"); return 1; }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(server_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        perror("bind"); return 1;
    }
    if (listen(server_fd, 20) < 0) { perror("listen"); return 1; }

    std::cout << "Backend listening on " << port << "\n";

    while (true) {
        sockaddr_in client{};
        socklen_t len = sizeof(client);
        int client_fd = accept(server_fd, reinterpret_cast<sockaddr*>(&client), &len);
        if (client_fd < 0) { perror("accept"); continue; }

        char buffer[4096]{};
        ssize_t n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

        if (n > 0) {
            std::string request(buffer, n);
            std::cout << "[" << port << "] " << request;

            std::string response = "BACKEND=" + std::to_string(port) + "\nREQUEST=" + request;
            send(client_fd, response.data(), response.size(), 0);
        }
        close(client_fd);
    }
}
