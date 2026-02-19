@0xf6827afa7700e3c2;

struct Person {
  id @0 :UInt32;
  name @1 :Text;
  email @2 :Text;

  struct PhoneNumber {
    number @0 :Text;

    enum Type {
      mobile @0;
      home @1;
      work @2;
    }

    type @1 :Type;
  }

  phones @3 :List(PhoneNumber);
}

struct AddressBook {
  people @0 :List(Person);
}

struct CanFrame {
  id        @0 :UInt32;
  extended  @1 :Bool;
  rtr       @2 :Bool;
  dlc       @3 :UInt8;
  data      @4 :Data;  # up to 8 bytes for classical CAN
  timestamp @5 :UInt64;
}
