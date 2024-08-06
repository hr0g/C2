#include <iostream>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <fcntl.h>
#include <sys/select.h>
#include <netinet/tcp.h>

std::mutex coutMutex;
int targetClient = -1;

void setSocketNonBlocking(int sock) {
    int flags = fcntl(sock, F_GETFL, 0);
    if (flags < 0) return;
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);
}

void sendMessage(int sock) {
    std::string message;
    while (true) {
        std::getline(std::cin, message);
        //if (message == "bye") break;  // Add a way to break the loop
        if (message == "exit")
        {
            targetClient = -1;
        }
        if (strncmp(message.c_str(), "hs", 2) == 0)
        {
            const char* digits = message.c_str() +2;
            while (*digits && std::isspace(*digits)) {
                ++digits;
            }
            if (*digits) {  // Make sure it's not just an empty string
                // Process the digits
                targetClient = atoi(digits);  // Convert to an integer
            }
        }
        message = std::string("hr0g") + message;

        if (send(sock, message.c_str(), message.length(), 0) < 0) {
            std::cerr << "Send failed: " << strerror(errno) << std::endl;
        }
    }
}

void displayReceivedMessage(const std::string& message) {
    std::lock_guard<std::mutex> lock(coutMutex);
    if (targetClient != -1)
    {
        std::cout << message << std::endl;
        std::cout << "┌──(root♪hr0g) - [~" << targetClient << "]" << std::endl << "└─# " << std::flush;
    }
    else
    {
        std::cout << message << std::endl;
        std::cout << "┌──(root♪hr0g) - [~]" << std::endl << "└─# " << std::flush;
    }
}

void receiveMessage(int sock) {
    fd_set readfds;
    struct timeval tv;
    int retval;
    char buffer[1024];

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);

        // Set timeout
        tv.tv_sec = 5;
        tv.tv_usec = 0;

        retval = select(sock + 1, &readfds, NULL, NULL, &tv);

        if (retval == -1) {
            perror("select()");
            break;
        }
        else if (retval > 0) {
            if (FD_ISSET(sock, &readfds)) {
                memset(buffer, 0, sizeof(buffer));
                int bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0); // Ensure buffer is null-terminated
                if (bytesReceived > 0) {
                    buffer[bytesReceived] = '\0'; // Null-terminate the buffer
                    std::string receivedData(buffer);
                    displayReceivedMessage(receivedData);
                }
                else if (bytesReceived == 0) {
                    // Connection closed by peer
                    std::cout << "Connection closed by peer" << std::endl;
                    break;
                }
                else {
                    // Error occurred
                    std::cerr << "recv() error: " << strerror(errno) << std::endl;
                    break;
                }
            }
        }
    }
}

int main() {
    int sock;
    struct sockaddr_in server;

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        std::cerr << "Could not create socket\n";
        return 1;
    }

    // Set TCP_NODELAY to disable Nagle's algorithm and send data immediately
    int flag = 1;
    int result = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int));
    if (result < 0) {
        std::cerr << "Error setting TCP_NODELAY\n";
        return 1;
    }

    server.sin_addr.s_addr = inet_addr("154.12.177.190");
    server.sin_family = AF_INET;
    server.sin_port = htons(5000);

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("connect failed. Error");
        return 1;
    }

    setSocketNonBlocking(sock);  // Set the socket to non-blocking

    std::cout << "Connected to the server.\n";
    //std::cout << "┌──(root♪hr0g) - [~]" << std::endl << "└─# " << std::flush;

    std::string hr0g("hr0g");
    send(sock, hr0g.c_str(), hr0g.length(), 0);

    // Start the sender and receiver threads
    std::thread sender(sendMessage, sock);
    std::thread receiver(receiveMessage, sock);

    sender.join();
    receiver.join();

    close(sock);
    return 0;
}
