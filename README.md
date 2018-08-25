tracker
===

asset tracking functionality from a serial connection


### events

- `R` - device ready
  - allows the server side to perform any device specific initialization needed prior to handling events
- `G` - gps event in format of `lat:lon`
