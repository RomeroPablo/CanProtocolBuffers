#include <capnp/message.h>
#include <capnp/serialize.h>
#include <capnp/serialize-packed.h>

#include <fcntl.h>
#include <thread>
#include <unistd.h>

#include <chrono>
#include <iostream>

#include "schema/can.capnp.h"
#include "tcp.hpp"

#define PORT 5052
#define IP "127.0.0.1"
void writeCanMessage(uint32_t fd) {
  capnp::MallocMessageBuilder message;
  CanFrame::Builder canFrame = message.initRoot<CanFrame>();

  uint8_t frame[8] = {0xAA, 0xBB, 0xCC, 0xDD, 0x00, 0x00, 0x00, 0x00};
  canFrame.setId(0x123);
  canFrame.setDlc(0x08);
  canFrame.setExtended(false);
  canFrame.setRtr(false);
  canFrame.setTimestamp(static_cast<std::uint64_t>(
                        std::chrono::duration_cast<std::chrono::milliseconds>((
                        std::chrono::system_clock::now()).time_since_epoch()).count()));

  canFrame.setData(kj::arrayPtr(reinterpret_cast<const capnp::byte*>(frame), sizeof(frame)));
  writePackedMessageToFd(fd, message);
}

void readCanMessage(uint32_t fd){
    capnp::PackedFdMessageReader message(fd);
    CanFrame::Reader canFrame = message.getRoot<CanFrame>();
    std::cout << "received message from 0x" << std::hex << canFrame.getId()
              << " recorded at time 0x" << std::hex << canFrame.getTimestamp() << std::endl;
}

void producer(){
    Tcp connection;
    connection.createConnection(PORT, NULL);
    while(1){
        writeCanMessage(connection.client_fd);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void consumer(){
    Tcp connection;
    connection.createConnection(PORT, IP);
    while(1){
        readCanMessage(connection.sock);
    }
}

int main() {
    std::thread producer_t(producer);
    std::thread consumer_t(consumer);
    producer_t.join();
    consumer_t.join();
    return 0;
}
