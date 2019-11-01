#include <Particle.h>
#include <functional>
#include <experimental/optional>

#include <NMEAGPS.h>

auto gpsPort = Serial1;

using meters = double;
using tick = system_tick_t;

using std::experimental::nullopt;
using std::experimental::optional;
using std::experimental::make_optional;

using Location = NeoGPS::Location_t;
using OptLocation = optional<Location>;

bool movedAtLeast(meters, Location&, Location&);
bool waitedAtLeast(tick, tick, const std::function<tick(void)>&);

SYSTEM_THREAD(ENABLED)

#ifdef ASSET_TRACKER
constexpr int serial1br = 9600;
#else
constexpr int serial1br = 38400;
#endif

constexpr int buffer_sz = 32;
std::array<char, buffer_sz> buffer;

NMEAGPS gps;
gps_fix fix;

tick lastEvent = 0;
tick eventInterval = 5000;

OptLocation lastPos = nullopt;
meters moveThreshold = 2;

const String Id = String(CloudClass::deviceID()).substring(0, 6);
const String ReadyEvent = String("R/") + Id;
const String MovedEvent = String("M/") + Id;

void setup() {
   gpsPort.begin(serial1br);

#ifdef ASSET_TRACKER
   // turn on the asset tracker gps
   pinMode(D6, OUTPUT);
   digitalWrite(D6, LOW);

   constexpr uint8_t ExternalAntenna[] = {0xB5, 0x62, 0x06, 0x13, 0x04, 0x00, 0x01, 0x00, 0xF0, 0x7D, 0x8B, 0x2E};
   gpsPort.write(ExternalAntenna, sizeof(ExternalAntenna));
#endif

   Particle.publish(ReadyEvent, PRIVATE);
}

void loop() {
   while(gps.available(gpsPort)) {
      fix = gps.read();

      if(fix.valid.location) {
         if(!lastPos || movedAtLeast(moveThreshold, *lastPos, fix.location) || waitedAtLeast(lastEvent, eventInterval, millis)) {
            auto lat = String(fix.location.latF(), 6);
            auto lon = String(fix.location.lonF(), 6);
            auto mph = String(static_cast<uint8_t>(fix.speed_mph()));
            auto str = String::format("%s:%s:%s", lat.c_str(), lon.c_str(), mph.c_str());

            lastEvent = millis();
            lastPos = make_optional(fix.location);
            Particle.publish(MovedEvent, str, PRIVATE);
         }
      }
   }
}

bool movedAtLeast(meters m, Location& from, Location& to) {
   return m <= from.DistanceKm(to) * 1000;
}

bool waitedAtLeast(tick prev, tick interval, const std::function<tick(void)>& t) {
   return t() - prev > interval;
}
