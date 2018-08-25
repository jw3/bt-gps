#include <Particle.h>
#include <TinyGPS++.h>
#include <functional>
#include <experimental/optional>

#ifdef ASSET_TRACKER
#include <LIS3DH.h>
#endif

using std::experimental::nullopt;
using std::experimental::optional;
using std::experimental::make_optional;
using OptLocation = optional<TinyGPSLocation>;

bool movedAtLeast(double, TinyGPSLocation&, TinyGPSPlus&);
bool waitedAtLeast(system_tick_t, system_tick_t, const std::function<system_tick_t(void)>&);

SYSTEM_THREAD(ENABLED)

#ifdef ASSET_TRACKER
constexpr int serial1br = 9600;
#else
constexpr int serial1br = 38400;
#endif

constexpr int buffer_sz = 32;
std::array<char, buffer_sz> buffer;

system_tick_t lastEvent = 0;
system_tick_t eventInterval = 5000;

OptLocation lastPos = nullopt;
double moveThreshold = 2;

TinyGPSPlus gps;

#ifdef ASSET_TRACKER
LIS3DHSPI accel(SPI, A2, WKP);
#endif

void setup() {
   Serial1.begin(serial1br);

#ifdef ASSET_TRACKER
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

bool movedAtLeast(double m, TinyGPSLocation& from, TinyGPSPlus& gps) {
   return m <= TinyGPSPlus::distanceBetween(
         gps.location.lat(), gps.location.lng(),
         from.lat(), from.lng());
}

bool waitedAtLeast(system_tick_t prev,
                   system_tick_t interval,
                   const std::function<system_tick_t(void)>& t) {
   return t() - prev > interval;
}
