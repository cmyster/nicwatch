# nicwatch

A NIC health journaling tool for Linux that monitors interface statistics.

## Build
```bash
make
```
## Usages
* ```nicwatch snap``` — take a snapshot of all interfaces (rx/tx errors, CRC errors, drops, carrier transitions, speed/duplex, link state) and store it with a timestamp in ~/.nicwatch/
* ```nicwatch status``` — show a one-liner-per-interface summary with color-coded health (green/yellow/red based on error rate trends)
* ```nicwatch history <iface>``` — show the timeline of an interface: when errors started climbing, when carrier bounced, when speed/duplex changed
* ```nicwatch diff [<timestamp>]``` — diff current state against a previous snapshot, highlighting what changed
* ```nicwatch watch``` — daemon mode, snap every N seconds, log to journal, optionally alert (write to syslog or run a hook script) when error counters spike
