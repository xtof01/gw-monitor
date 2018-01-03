gw-monitor - Default Gateway Monitor
=====================================

**gw-monitor** monitors the routing table and checks if the default
route points to the interface specified as command-line parameter. As
soon as this is the case, it plays a sound on the PC speaker device and
turns on the second (i.e. middle one) of the APU2 onboard LEDs.

If the default route gets deleted (e.g. because the link was lost
or disconnected) another sound is played and the LED is switched off.
