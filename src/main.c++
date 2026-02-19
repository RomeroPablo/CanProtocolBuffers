#include <capnp/message.h>
#include <capnp/serialize-packed.h>

#include <fcntl.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <string>

#include "schema/can.capnp.h"

static void writeAddressBook(const std::string& path) {
  ::capnp::MallocMessageBuilder message;
  AddressBook::Builder addressBook = message.initRoot<AddressBook>();

  ::capnp::List<Person>::Builder people = addressBook.initPeople(1);
  Person::Builder alice = people[0];

  alice.setId(123);
  alice.setName("Alice Example");
  alice.setEmail("alice@example.com");

  ::capnp::List<Person::PhoneNumber>::Builder phones = alice.initPhones(2);
  phones[0].setNumber("+1-555-0100");
  phones[0].setType(Person::PhoneNumber::Type::MOBILE);

  phones[1].setNumber("+1-555-0101");
  phones[1].setType(Person::PhoneNumber::Type::WORK);

  int fd = ::open(path.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0644);
  if (fd < 0) {
    throw std::runtime_error(
        "Failed to open output file: " + path + ": " + std::strerror(errno));
  }

  try {
    writePackedMessageToFd(fd, message);
  } catch (...) {
    ::close(fd);
    throw;
  }

  if (::close(fd) != 0) {
    throw std::runtime_error("Failed to close output file: " + path);
  }
}

static void readAddressBook(const std::string& path) {
  int fd = ::open(path.c_str(), O_RDONLY);
  if (fd < 0) {
    throw std::runtime_error("Failed to open input file: " + path);
  }

  ::capnp::PackedFdMessageReader message(fd);
  AddressBook::Reader addressBook = message.getRoot<AddressBook>();

  for (Person::Reader person : addressBook.getPeople()) {
    std::cout << "Person\n";
    std::cout << "  id: " << person.getId() << "\n";
    std::cout << "  name: " << person.getName().cStr() << "\n";
    std::cout << "  email: " << person.getEmail().cStr() << "\n";

    for (Person::PhoneNumber::Reader phone : person.getPhones()) {
      const char* type = "unknown";
      switch (phone.getType()) {
        case Person::PhoneNumber::Type::MOBILE:
          type = "mobile";
          break;
        case Person::PhoneNumber::Type::HOME:
          type = "home";
          break;
        case Person::PhoneNumber::Type::WORK:
          type = "work";
          break;
      }

      std::cout << "  phone (" << type << "): " << phone.getNumber().cStr() << "\n";
    }
  }

  if (::close(fd) != 0) {
    throw std::runtime_error("Failed to close input file: " + path);
  }
}

int main() {
  const std::string outputFile = "artifacts/can.bin";

  std::filesystem::create_directories("artifacts");

  try {
    writeAddressBook(outputFile);
    std::cout << "Wrote " << outputFile << "\n\n";

    readAddressBook(outputFile);
  } catch (const std::exception& ex) {
    std::cerr << "Error: " << ex.what() << "\n";
    return 1;
  }

  return 0;
}
