#pragma once

struct Receiver
{
   explicit Receiver(USARTSerial& serial) : serial(serial) {}

   void begin(int baudrate) {
      serial.begin(baudrate);
   }

   void transmit(const char* e, const char* d) {
      serial.printlnf("%s:%s", e, d);
   }
private:
   USARTSerial& serial;
};
