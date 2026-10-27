#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <string>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: ./client <client_id> <vlan> [message]\n";
        return 1;
    }

    int client_id = std::stoi(argv[1]);
    int vlan = std::stoi(argv[2]);
    std::string message = argc >= 4 ? argv[3] : "GET_DATA";

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) { perror("socket"); return 1; }

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(9000);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    if (connect(fd, reinterpret_cast<sockaddr*>(&server), sizeof(server)) < 0) {
        perror("connect"); return 1;
    }

    std::string request = "CLIENT_ID=" + std::to_string(client_id) +
                           "\nVLAN=" + std::to_string(vlan) +
                           "\nREQUEST=" + message + "\n";

    send(fd, request.data(), request.size(), 0);

    char buffer[4096]{};
    ssize_t n = recv(fd, buffer, sizeof(buffer) - 1, 0);
    if (n > 0) std::cout << buffer;

    close(fd);
    return 0;
}
