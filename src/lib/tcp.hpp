#pragma once

#include <cstdint>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <system_error>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <thread>

void inline tLog(std::string s){
    const auto id = std::this_thread::get_id();
    std::cout << "[0x" << std::hex << id << "] " << s << std::endl;
};

struct Tcp{
    int createConnection(uint32_t port, const char* IP);
    int closeConnection();
    int writePacket(std::vector<uint8_t>& buffer);
    int readPacket(std::vector<uint8_t>& buffer);
    uint32_t sock = -1;
    uint32_t client_fd = -1;
};

int Tcp::writePacket(std::vector<uint8_t>& buffer){
    return send(sock, buffer.data(), buffer.size(), 0);
}

int Tcp::readPacket(std::vector<uint8_t>& buffer){
    return recv(client_fd, buffer.data(), buffer.size(), 0);
};

int Tcp::createConnection(uint32_t port, const char* IP){
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    int opt = 1;
    tLog("Tcp Socket created");
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    struct sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if(IP == NULL){ // server
    if(bind(sock, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) < 0){
        close(sock);
        tLog("Tcp bind failed");
        throw std::system_error(errno, std::system_category(), "bind");
    }
    if(listen(sock, 32) < 0){
        close(sock);
        tLog("Tcp listen failed");
        throw std::system_error(errno, std::system_category(), "listen");
    }
    client_fd = accept(sock, NULL, NULL);
    return 0;
    }

    if(inet_pton(AF_INET, IP, &serverAddr.sin_addr) < 0){
        close(sock);
        tLog("Tcp inet_pton failed");
        throw std::system_error(errno, std::system_category(), "inet_pton");
    }
    if(connect(sock, reinterpret_cast<sockaddr*>(&serverAddr),sizeof(serverAddr)) < 0){
        close(sock);
        tLog("Tcp connect failed");
        throw std::system_error(errno, std::system_category(), "connect");
    }
    return 0;
};

int Tcp::closeConnection(){
    return close(sock);
};
