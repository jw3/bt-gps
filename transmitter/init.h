#pragma once

auto gpsPort = Serial1;

#ifdef ASSET_TRACKER
constexpr int gpsBr = 9600;
#else
constexpr int gpsBr = 38400;
#endif

void init() {
   gpsPort.begin(gpsBr);

#ifdef ASSET_TRACKER
   // turn on the asset tracker gps
   pinMode(D6, OUTPUT);
   digitalWrite(D6, LOW);

   constexpr uint8_t ExternalAntenna[] = {0xB5, 0x62, 0x06, 0x13, 0x04, 0x00, 0x01, 0x00, 0xF0, 0x7D, 0x8B, 0x2E};
   gpsPort.write(ExternalAntenna, sizeof(ExternalAntenna));
#endif
}
