tracker
===

asset tracker type functionality


### events

- `R` - device ready
  - allows the server side to perform any device specific initialization needed prior to handling events
- `G` - gps event in format of `lat:lon`


### firmware
- bt-serial: provides a basic gps ping taken from a serial connection, throttled with time and distance thresholds
  - this is not intended to be used on an asset tracker, specifically designed for a nmea connection over bluetooth
- asset-tracker: gps ping intended for use on the particle asset tracker boards
