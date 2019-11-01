#include <Particle.h>
#include <rx.h>

constexpr int baudrate = 9600;
Receiver rx(Serial1);

void setup() {
   rx.begin(baudrate);
   Particle.subscribe("M", &Receiver::transmit, &rx, MY_DEVICES);
}
