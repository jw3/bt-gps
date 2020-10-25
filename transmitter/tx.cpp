#include <Particle.h>
#include <functional>
#include <experimental/optional>

#include <NMEAGPS.h>
#include <init.h>

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

constexpr int buffer_sz = 32;
std::array<char, buffer_sz> buffer;

NMEAGPS gps;
gps_fix fix;

constexpr tick EventInterval = 5000;
constexpr tick MaxConnectDuration = 10000;
constexpr tick ConnectPauseDuration = 1000;

tick lastEvent = 0;
tick disconnectedAt = 0;
tick connectingSince = 0;

OptLocation lastPos = nullopt;
meters moveThreshold = 3;

const String Id = String(CloudClass::deviceID()).substring(0, 6);
const String ReadyEvent = String("R/") + Id;
const String MovedEvent = String("M/") + Id;

void setup() {
   init();
   Particle.publish(ReadyEvent, PRIVATE);
}

// state machine ...
void loop() {
   if(Cellular.ready()) {
      if(connectingSince) {
         // log?
         connectingSince = 0;
      }

      while(gps.available(gpsPort)) {
         fix = gps.read();

         if(fix.valid.location) {
            if(!lastPos || movedAtLeast(moveThreshold, *lastPos, fix.location) || waitedAtLeast(lastEvent, EventInterval, millis)) {
               auto x = String(fix.location.lonF(), 6);
               auto y = String(fix.location.latF(), 6);
               auto str = String::format("%s:%s", x.c_str(), y.c_str());

               lastEvent = millis();
               lastPos = make_optional(fix.location);
               Particle.publish(MovedEvent, str, PRIVATE);
            }
         }
      }
   }
   else {
      if(connectingSince && waitedAtLeast(connectingSince, MaxConnectDuration, millis)) {
         if(!Cellular.ready()) {
            Cellular.disconnect();
            disconnectedAt = millis();
         }
         connectingSince = 0;
      }
      else if(disconnectedAt && waitedAtLeast(disconnectedAt, ConnectPauseDuration, millis)) {
         Cellular.connect();
         connectingSince = millis();
         disconnectedAt = 0;
      }
   }
}

bool movedAtLeast(meters m, Location& from, Location& to) {
   return m <= from.DistanceKm(to) * 1000;
}

bool waitedAtLeast(tick prev, tick interval, const std::function<tick(void)>& t) {
   return t() - prev > interval;
}
