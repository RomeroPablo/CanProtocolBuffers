@0xf6827afa7700e3c2;

struct CanFrame {
  id        @0 :UInt32;
  extended  @1 :Bool;
  rtr       @2 :Bool;
  dlc       @3 :UInt8;
  timestamp @4 :UInt32;
  data      @5 :Data;  # up to 8 bytes for classical CAN
}
