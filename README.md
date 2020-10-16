tracker
===

### events

- `R` - device ready
  - allows the server side to perform any device specific initialization needed prior to handling events
- `M` - gps event in format of `x:y:s`
  - x: lon
  - y: lat
  - s: speed in miles per hours
