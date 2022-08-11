This project implements a splitter for the control signal for WS2812 LED chains.

Why
===

WS2812 LEDs are RGB controllable leds that can be chained from a single wire control signal source.
These LED chains have become quite ubiquitous in a variety of devices. Unfortunately, they suffer from an issue
where a single LED failing in the chain can cause all LEDS after them to fail. This causes large chains of
these LEDs to be quite failure prone.

This ended up being quite a problem with device I've been working on, which contained numerous led strips using these.
Unfortunately, the manufacturing of these strips turned out to be rather shoddy (either a bad bad of LEDs, or just bad soldering)
and the device would suffer single led failures very often. After having to take it apart one too many times to fix them we decided
that a more permanent solution was necessary. Looking at the protocol used by the LEDs I realized that it should be possible for
an inexpensive piece of hardware to intercept the signal and split it over multiple feed points so a single failure would no longer make
the device inoperable.

Hardware
========

I chose the [Arduino Nano Every](LINKHERE) board as it was the cheapest off-the-shelf board I could find that would require only cabling work to
integrate it. The protocol used by the leds requires +/-150ns precision, which ruled out the slightly cheaper but not as fast Arduino Nano.
Additionally, these boards can be programmed using the quite ubiquitous Arduino IDE straight over USB, eliminating the need for a programmer.

Note: for some reason the Arduino IDE programs the board to run on a 16MHz clock by default, instead of the intended 20MHz clock. You probably
will need to edit the relevant boards.txt file in the arduino IDE to edit the relevant fuse value and F_CPU setting, otherwise the code
will not compile.

Software
========

The software to run the splitter can be found in `ledstripesplitter/ledstripesplitter.ino`. At the top of the file there's a small section of configuration
defines that can be altered to suit different led configurations. One major limitation of the splitter is that all segments must be of the same length, outside of that
all GPIO pins of the device can be used as input or output pins. Programming the device is simple, load the project into the Arduino IDE, select the Arduino nano every board (Atmega4809) and program it.

Wiring
======

The way you wire this thing up depends on the exact led strip geometry you want to control, but I'll describe an example here.
Any resemblance between this example and a WACCA arcade cab is purely accidental.

So for this example we'll be modifying a led strip containing 480 LEDs. This device has these strips structured into 12 logical segments of 40 LEDs.
The input signal is provided as a ground and signal wire. The actual connector on the LED pcb side also has a 5V power pin exposed, which we can use
to power the splitter device directly. The signal will be fed to each segment by removing the signal bridge between segments and instead driving each
segment directly from the splitter.

This results in the connections to the microcontroller being as follows:


```
                        +--------[ USB ]--------+
                        |                       |
                        + PE2               PE1 +
                        |                       |
                        + 3v3               PE0 +
                        |                       |
                        + PD7               PB1 +
                        |                       |
                        + PD3               PB0 +---- segment 12
                        |                       |
                        + PD2               PE3 +---- segment 11
                        |                       |
                        + PD1               PA1 +---- segment 10
                        |                       |
        segment 5   ----+ PD0               PF4 +---- segment 9
                        |                       |
        segment 4   ----+ PA2               PB2 +---- segment 8
                        |                       |
        segment 3   ----+ PA3               PC6 +---- segment 7
                        |                       |
        segment 2   ----+ PD4               PF5 +---- segment 6
                        |                       |
        segment 1   ----+ PD5               PA0 +---- input signal
                        |                       |
                        + 5Vout             GND +---- input ground
                        |                       |
                        + \RST             \RST +
                        |                       |
        LED ground  ----+ GND               PC5 +
                        |                       |
        LED power   ----+ Vin               PC4 +
                        |                       |
                        +-----------------------+
```

That's all! Have fun!
