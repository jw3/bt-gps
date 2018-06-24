#include <Particle.h>
#include <TinyGPS++.h>
#include <experimental/optional>

SYSTEM_THREAD(ENABLED)

using std::experimental::nullopt;
using std::experimental::optional;
using std::experimental::make_optional;
using OptLocation = optional<TinyGPSLocation>;

constexpr int buffer_sz = 32;
std::array<char, buffer_sz> buffer;

system_tick_t lastEvent = 0;
system_tick_t eventInterval = 5000;

OptLocation lastPos = nullopt;
double moveThreshold = 2;

TinyGPSPlus gps;

bool movedAtLeast(double m, TinyGPSLocation from) {
   return m <= TinyGPSPlus::distanceBetween(
         gps.location.lat(), gps.location.lng(),
         from.lat(), from.lng());
}

bool waitedAtLeast(system_tick_t t) {
   return millis() - lastEvent > t;
}

void setup() {
   Serial1.begin(9600);
   pinMode(D6, OUTPUT);
   digitalWrite(D6, LOW);

   Particle.publish("R", PRIVATE);
}

void serialEvent1() {
   Serial1.readBytes(buffer.data(), buffer_sz);
   for(const auto& c: buffer) {
      if(gps.encode(c) && waitedAtLeast(eventInterval)) {
         auto location = gps.location;
         if(location.isValid() && location.isUpdated()) {
            if(!lastPos || movedAtLeast(moveThreshold, *lastPos)) {
               auto lat = String(location.lat(), 6);
               auto lon = String(location.lng(), 6);
               auto str = String::format("%s:%s", lat.c_str(), lon.c_str());

               lastEvent = millis();
               lastPos = make_optional(location);
               Particle.publish("G", str, PRIVATE);
            }
         }
      }
   }
   buffer = {};
}
