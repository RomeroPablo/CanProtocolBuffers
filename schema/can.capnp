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

struct CANMessage {
    id @0 :UInt32;
    dlc @1 : UInt32;
    data @2 : List(UInt32);
}
