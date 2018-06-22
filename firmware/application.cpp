#include <Particle.h>
#include <TinyGPS++.h>

constexpr int buffer_sz = 32;

system_tick_t lastEvent = 0;
system_tick_t eventInterval = 5000;
std::array<char, buffer_sz> buffer;

TinyGPSPlus gps;

void setup() {
   Serial1.begin(38400);
   Particle.publish("R", PRIVATE);
}

void loop() {
   while(Serial1.available() > 0) {
      if(Serial1.readBytes(buffer.data(), buffer_sz)) {
         bool exec = millis() - lastEvent > eventInterval;
         for(const auto& c: buffer) {
            if(gps.encode(c) && exec) {
               auto location = gps.location;
               if(location.isValid() && location.isUpdated()) {
                  auto lat = String(location.lat(), 6);
                  auto lon = String(location.lng(), 6);
                  auto str = String::format("%s:%s", lat.c_str(), lon.c_str());

                  lastEvent = millis();
                  Particle.publish("G", str, PRIVATE);
               }
            }
         }
      }
      buffer = {};
   }
}
