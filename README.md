# USBLRS - Wireless FPV simulator adapter
A small device that allows the use of ELRS, Ghost, Crossfire, etc. receivers with any device that supports HID inputs over USB.
In simpler words: a wireless FPV simulator adapter. No drivers, low-latency,  plug-and-play.

# Hardware
- Any Seeeduino XIAO board (only RP2040 tested to date)
- Any receiver that supports an inverted S.Bus/SBUS output
- Optional PCB

# Getting started
0. I suggest ordering the PCB (gerber_v1.0.zip) from a service like JLCPCB or PCBWay, it's cheap af
1. Flash the firmware to the XIAO using Arduino IDE (many tutorials online)
2. Solder straight to the PCB, or wire it up according to this:
    **XIAO > Receiver**
    5V > 5V
    GND > GND
    D6 > RX
    D7 > TX
3. Plug in and verify with https://hardwaretester.com/gamepad
4. Enjoy

----------------------------

# V1.0
The first version, the simplest and quickest for me to make, was using an ImmersionRC Ghost receiver in inverted S.Bus mode. The fact that it has that feature allowed me to use Seeed XIAO RP2040's non-inverted UART RX pin together with S.Bus, which was the simplest protocol to decode in the moment. It should work with all Seeeduino XIAO boards without modifications, and other development boards with Arduino support by changing the TX/RX pins in code. All receivers with an inverted S.Bus feature should work.

**Tested hardware:**
- XIAO RP2040
- IRC Ghost Atto

----------------------------

# License
USBLRS is available under the MIT license. See the LICENSE file for more info.
