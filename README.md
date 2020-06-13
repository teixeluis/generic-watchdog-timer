# generic-watchdog-timer

## Overview

A hardware watchdog timer for devices featuring a serial port

This project introduces a standalone watchdog timer for use with
any device featuring one availabe serial port (RS-232), and having the 
capability of periodically sending a specific string of data during its 
normal operation.

This project is an improvement over my previous project, the 
[hardware-watchdog-timer](https://github.com/teixeluis/hardware-watchdog-timer), 
given that it allows for a more complex input to cause the watchdog to reset. 
More specifically, it requires a particular string of characters to be received, 
to cause the timer to reset, and as such prevent the target device from being 
restarted.

This watchdog timer (WDT) is also capable of sending commands back to the 
target system. In the current implementation, the WDT is sending a shutdown
command to the target device, in order to allow it to shutdown properly, 
prior to being turned off.

Combined with a relay, this device is then capable of cycling the power
of the target device in order to (hard) restart it in case of failure.

This source code is accompanied with an application schematic diagram to
help in the construction of a complete unit using the PIC16F628A microcontroller
and this firmware.


## Technical description

In this implementation we are using the TIMER1 of this PIC microcontroller. From the
three regular timers available in this device, this is the only 16 bit one. For our application
it is ideal that the timer has this level of resolution, as it allows for a more
accurate definition of the duration of the timer. Depending on the application it may
also be better when compared to the 8 bit timers, in respect to the reduction of instruction
loops, when longer timer durations are required.

Whenever the timer overflows, an interrupt is generated, and our interrupt service routine (isr)
takes care of incrementing a counter variable or producing the WDT action (sending a command to
the target device and briefly turning off and on the relay) when this variable overflows. 
Then TIMER1 is restarted and the cycle repeats.

On the other hand, while the timer is running, a software loop continuously reads from the serial
port, comparing the stream of bytes with the expected string ("kiosk_wdtToken"). Everytime a
match is found, the counter variable of the timer is reset to 0, preventing the triggering of
the WDT.

I have selected this particular microcontroller due to the fact that I was already familiar 
with the architecture, and this model in particular while being cheap, has a useful feature set.
One such example is the integrated USART which besides other modes of operation, it can be 
configured to behave as a regular RS-232 serial port. Regarding the later, one caveat that 
this microcontroller presents, is at configuring an accurate enough approximation of the required
baud rate. In the particular application for which I developed this WDT, I first intended to
use the internal clock oscilator, which tops at 4 MHz. But for producing 115200, at this clock
speed, the closest rate that is possible to obtain is 125000, which is quite a long shot.
As such the only option in this case was to add an external 20 MHz crystal, and configure the
clock to run at that speed. At this clock rate, the USART can generate 113 636 bps, which
for most purposes is close enough (only 1.36 % off).

Regarding the voltage levels of the serial port, in my application I powered the PIC at 5 Volts,
so that necessarily the TX and RX pins would output and expect TTL levels.

I have attempted to run the device at 3.3 Volts but it was not stable enough because of the high
clock frequency. So in my particular case, as I needed to interface with a device using CMOS 
(3.3 Volt) levels, a level shifter was required.

## Building and flashing

This project can be built on the MPLAB X IDE. It requires the XC8 toolchain for compiling 
the firmware image.


For building it can simply be opened in the IDE, and launch the project build. Alternatively 
you can the programming of the device directly, if you have a PIC ready to be flashed in your 
programmer. It should be possible to use practically any Microchip programmer to flash the
device (e.g. a PICkit 2 or 3).

## Pin mappings of the PIC16F628A microcontroller

The diagram below shows the pin mappings of the PIC16F628A microntroller 
used  for this particular application:

```
                    +---------------+
  N/A         -   --|(RA2)|   |(RA1)|--   > RELAY
                    |     \---/     |
  N/A         -   --|(RA3)     (RA0)|--   - N/A
                    |               |
  N/A         -   --|(RA4)     (RA7)|--   - XTAL
                    |               |
  N/A         -   --|(RA5)     (RA6)|--   - XTAL
                    |               |
  GND         -   --|(VSS)     (VDD)|--   - +5V
                    |               |
  N/A         -   --|(RB0)     (RB7)|--   - N/A
                    |               |
  RX          >   --|(RB1)     (RB6)|--   - N/A
                    |               |
  TX          <   --|(RB2)     (RB5)|--   - N/A
                    |               |
  N/A         -   --|(RB3)     (RB4)|--   - N/A
                    +---------------+
```

## License

Author: Luis Teixeira (https://creationfactory.co)

Licence and copyright notice:

Copyright 2020 Luis Teixeira

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file 
except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, softwar distributed under the License 
is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express 
or implied. See the License for the specific language governing permissions and limitations 
under the License.

You probably can have success with one of the Arduino based programmers that are around, but 
I haven't personally tested.
