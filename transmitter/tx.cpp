#include <Particle.h>
#include <TinyGPS++.h>
#include <functional>
#include <experimental/optional>

#ifdef ASSET_TRACKER
#include <LIS3DH.h>
#endif

using meters = double;
using tick = system_tick_t;
using std::experimental::nullopt;
using std::experimental::optional;
using std::experimental::make_optional;
using OptLocation = optional<TinyGPSLocation>;

bool movedAtLeast(meters, TinyGPSLocation&, TinyGPSPlus&);
bool waitedAtLeast(tick, tick, const std::function<tick(void)>&);

SYSTEM_THREAD(ENABLED)

#ifdef ASSET_TRACKER
constexpr int serial1br = 9600;
#else
constexpr int serial1br = 38400;
#endif

constexpr int buffer_sz = 32;
std::array<char, buffer_sz> buffer;

tick lastEvent = 0;
tick eventInterval = 5000;

OptLocation lastPos = nullopt;
meters moveThreshold = 2;

TinyGPSPlus gps;

#ifdef ASSET_TRACKER
LIS3DHSPI accel(SPI, A2, WKP);
#endif

void setup() {
   Serial1.begin(serial1br);

#ifdef ASSET_TRACKER
   // turn on the asset tracker gps
   pinMode(D6, OUTPUT);
   digitalWrite(D6, LOW);
#endif

   Particle.publish("R", PRIVATE);
}

void serialEvent1() {
   Serial1.readBytes(buffer.data(), buffer_sz);
   for(const auto& c: buffer) {
      if(gps.encode(c) && waitedAtLeast(lastEvent, eventInterval, millis)) {
         auto location = gps.location;
         if(location.isValid() && location.isUpdated()) {
            if(!lastPos || movedAtLeast(moveThreshold, *lastPos, gps)) {
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

void loop() {
}

bool movedAtLeast(meters m, TinyGPSLocation& from, TinyGPSPlus& gps) {
   return m <= TinyGPSPlus::distanceBetween(
         gps.location.lat(), gps.location.lng(),
         from.lat(), from.lng());
}

bool waitedAtLeast(tick prev, tick interval, const std::function<tick(void)>& t) {
   return t() - prev > interval;
}
