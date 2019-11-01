#pragma once

#include <receiver/rx.h>
#include <Serial4/Serial4.h>

#ifdef ASSET_TRACKER
constexpr int gpsBr = 9600;
#else
constexpr int gpsBr = 38400;
#endif

constexpr int rxBr = 9600;

auto gpsPort = Serial1;
Receiver rx(Serial4);

void init() {
   gpsPort.begin(gpsBr);

   rx.begin(rxBr);
   Particle.subscribe("M", &Receiver::transmit, &rx, MY_DEVICES);
}
