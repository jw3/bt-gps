#include <Particle.h>

constexpr int serial1br = 9600;

void transmit(const char* e, const char* d) {
   Serial1.printlnf("%s:%s", e, d);
}

void setup() {
   Particle.subscribe("M", transmit, MY_DEVICES);
   Serial1.begin(serial1br);
}
