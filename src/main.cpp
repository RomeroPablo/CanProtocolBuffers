#include <capnp/message.h>
#include <capnp/serialize.h>
#include <capnp/serialize-packed.h>

#include <fcntl.h>
#include <unistd.h>

#include <iostream>

#include "schema/can.capnp.h"

void writeCanMessage(const std::string& fileBuffer) {
  capnp::MallocMessageBuilder message;
  CanFrame::Builder canFrame = message.initRoot<CanFrame>();

  uint8_t frame[8] = {0xAA, 0xBB, 0xCC, 0xDD, 0x00, 0x00, 0x00, 0x00};
  canFrame.setId(0x123);
  canFrame.setDlc(0x08);
  canFrame.setExtended(false);
  canFrame.setRtr(false);
  canFrame.setTimestamp(0);
  canFrame.setData(kj::arrayPtr(reinterpret_cast<const capnp::byte*>(frame), sizeof(frame)));

  int fd = open(fileBuffer.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0644);

  writePackedMessageToFd(fd, message);

  close(fd);
}

void readCanMessage(std::string fileBuffer){
    int fd = open(fileBuffer.c_str(), O_RDONLY);
    capnp::PackedFdMessageReader message(fd);
    CanFrame::Reader canFrame = message.getRoot<CanFrame>();
    std::cout << "0x" << std::hex << canFrame.getId() << std::endl;
}

int main() {
    const std::string outputFile = "artifacts/can.bin";
    writeCanMessage(outputFile);
    std::cout << "Wrote to " << outputFile << std::endl;
    readCanMessage(outputFile);
    std::cout << "Read from " << outputFile << std::endl;
    return 0;
}
