#include <iostream>
#include <cstring>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> // For close()
#include <thread>
#include <mutex>

std::vector<int> clientSockets; // Vector to store client sockets
std::mutex clientSocketsMutex;  // Mutex to protect access to clientSockets
int hostIndex = -1;

void ClientHandler(int clientSocket, int clientIndex) {
    char buffer[1024];
    bool hr0g = false;
    int bytesReceived;

    while (true) {
        bytesReceived = recv(clientSocket, buffer, 1024, 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0'; // Ensure null-terminated string
            if (strncmp(buffer, "hr0g", 4) == 0)
            {
                std::cout << "Received from Host " << clientIndex << ":\n" << buffer << std::endl;
                hr0g = true;
                hostIndex = clientIndex;
            }
            else
            {
                std::cout << "Received from Client " << clientIndex << ":\n" << buffer;
                hr0g = false;
            }
            // Send the message to all other clients
            if (clientIndex == hostIndex)
            {
                for (int i = 0; i < clientSockets.size(); ++i) {
                    if (i != clientIndex && clientSockets[i] != -1) { // Check socket is not the sender and is valid
                        send(clientSockets[i], buffer, bytesReceived, 0);
                    }
                }
            }
            else if (clientIndex != -1 && hostIndex != -1)
            {
                send(clientSockets[hostIndex], buffer, bytesReceived, 0);
            }
            clientSocketsMutex.unlock();
        }
        else {
            // Handle errors or disconnection
            clientSocketsMutex.lock();
            std::cout << "Client disconnected.\n";
            close(clientSocket);  // Close the socket of this client
            clientSockets[clientIndex] = -1; // Mark this client socket as closed
            clientSocketsMutex.unlock();
            break;
        }
    }
}

int main() {
    int serverSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrSize = sizeof(struct sockaddr_in);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Failed to create socket\n";
        return 1;
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(5000);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Bind failed\n";
        close(serverSocket);
        return 1;
    }

    listen(serverSocket, SOMAXCONN);
    std::vector<std::thread> threads;

    while (true) {
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrSize);
        if (clientSocket == -1) {
            std::cerr << "Accept failed\n";
            break; // Exit or handle error appropriately
        }

        std::cout << "Client connected.\n";

        clientSocketsMutex.lock();
        clientSockets.push_back(clientSocket); // Add new client socket to vector
        int clientIndex = clientSockets.size() - 1;
        clientSocketsMutex.unlock();

        threads.push_back(std::thread(ClientHandler, clientSocket, clientIndex));
    }

    for (auto& th : threads) {
        if (th.joinable()) {
            th.join();
        }
    }

    close(serverSocket);
    return 0;
}
