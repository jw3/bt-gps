#include <Particle.h>
#include <functional>
#include <experimental/optional>

#ifdef ASSET_TRACKER
#include <LIS3DH.h>
#endif

#include <NMEAGPS.h>
#include <GPSport.h>

auto gpsPort = Serial1;

constexpr uint8_t ExternalAntenna[] = {0xB5, 0x62, 0x06, 0x13, 0x04, 0x00, 0x01, 0x00, 0xF0, 0x7D, 0x8B, 0x2E};

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

tick lastEvent = 0;
tick eventInterval = 5000;

OptLocation lastPos = nullopt;
meters moveThreshold = 2;

const String Id = String(CloudClass::deviceID()).substring(0, 6);
const String ReadyEvent = String("R/") + Id;
const String MovedEvent = String("M/") + Id;

#ifdef ASSET_TRACKER
LIS3DHSPI accel(SPI, A2, WKP);
#endif


void setup() {
   gpsPort.begin(serial1br);

#ifdef ASSET_TRACKER
   // turn on the asset tracker gps
   pinMode(D6, OUTPUT);
   digitalWrite(D6, LOW);
   gpsPort.write(ExternalAntenna, sizeof(ExternalAntenna));
#endif

   Particle.publish(ReadyEvent, PRIVATE);
}

//--------------------------

NMEAGPS gps;
gps_fix fix;
void loop() {
   while(gps.available(gpsPort)) {
      fix = gps.read();

      if(fix.valid.location && waitedAtLeast(lastEvent, eventInterval, millis)) {
         auto l = gps.fix().location;
         if(!lastPos || movedAtLeast(moveThreshold, *lastPos, l)) {
            auto lat = String(l.latF(), 6);
            auto lon = String(l.lonF(), 6);
            auto str = String::format("%s:%s", lat.c_str(), lon.c_str());

            lastEvent = millis();
            lastPos = make_optional(l);
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
