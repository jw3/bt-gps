#include <Particle.h>
#include <TinyGPS++.h>

long lastEvent = 0;
int eventInterval = 5 * 1000;

TinyGPSPlus gps;

void setup() {
   Serial1.begin(38400);
   Particle.publish("ready", PRIVATE);
}

void loop() {
   while(Serial1.available() > 0) {
      auto v = Serial1.read();
      if(gps.encode(v)) {
         if(millis() - lastEvent > eventInterval) {
            auto location = gps.location;
            if(location.isValid() && location.isUpdated()) {
               auto lat = String(location.lat(), 6);
               auto lon = String(location.lng(), 6);
               auto str = String::format("%s:%s", lat.c_str(), lon.c_str());

               lastEvent = millis();
               Particle.publish("pos", str, PRIVATE);
            }
         }
      }
   }
}
