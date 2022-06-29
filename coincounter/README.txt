Project to hack an Asahi Seiko AF-754EF coin tester to accept various Euro coins instead.

In a project to hack a Konami card dispenser to accept euros instead we discovered the AF-754EF unit. While first expecting a simple mechanical tester, we instead discovered a fairly old, yet fairly advanced digital coin tester. This repo is dedicated to understanding the unit, reversing it and then modifying it to accept euros instead.

The reversing part can be found in the folder `schematics`. It contains several pictures of the unit with overlaid drawings of all traces, as well as a fully reversed schematic of the device. Additionally it has a schematic for an interface board to extract the prom of the used microcontroller.

The device uses a NEC UPD75P0076CU microcontroller. This is the DIP + PROM variant of the UPD750068 microcontroller. When looking for datasheets for the device searching for the latter yields better results. This is a 512 nibble RAM, 16kiB ROM 4-bit microcontroller with 42 pins, running (in this unit) at 4MHz. It contains all the code operating the device.

Connected to this microcontroller is a 93LC46B EEPROM chip. This is a 16-bit EEPROM containing 64 words of data for a total of 128B storage. This EEPROM contains the configuration data for the device, which specifies which coin(s) the device accepts.

There's one other IC of the device (an UCN5821), but that's just a simple IO expansion chip. It is used to drive the 7segment display.

An initial attempt was made to dump the contents of the EEPROM by using a logic analyzer on the communication lines between the EEPROM and the microcontroller. This was successful, but the contents of the EEPROM were not easily decipherable (script used: `eeprom_trace_decoder.py`). The resulting EEPROM dump (and all other data dumps mentioned hereafter can be found in the `data` folder).

Therefore, it proved necessary to dump the code running on the device. Luckily this proved rather easy as the datasheet specifies a PROM verification functionality that can be performed at any point after programming. This verification function reads out the entire PROM contents over an 8-bit parallel bus as clock pulses are fed into this device. Unfortunately this process requires some odd voltages to be applied to the chip which could not be realized in circuit, so the chip was carefully desoldered from the base PCB and placed into a socket soldered to a matrix board holding the interfacing circuitry. Another socket was soldered onto the base PCB so the chip could now be safely re-installed. This board proved a bit interesting due to the unconventional 1.77mm pitch used by the chip.

As the verification process uses a rather custom protocol a quick C++ program was coded to allow it to be read out via an Arduino Nano Every. This program (and the script to control it from the PC side) can be found in the `prom_dumper` folder, together with a basic schematic of the used matrix board.

For verification purposes the code was dumped twice, resulting in the dumps found in `data/prom_dumps`. After fixing a small hardware issue both dumps ended up matching, and the resulting code could be analysed. The chip uses the ill-known RA75X (or RA75XL) instruction set, but luckily the datasheet specified the encoding for (almost) all opcodes (it misses the encoding for a few, seemingly by mistake). A simple customizeable disassembler was written in `disassembler`, which also performed some basic call graph and jump analysis to come to the full program disassembly in `data/decompiled_code/dump.s`. This code was then decompiled by hand into the pseudocode that can be found in this folder as well.

The decompilation revealed the functionality of the device, clarifying the format of the EEPROM, as well as some helpful debugging functionality that could be activated by pulling two pins exposed on the side of the device to ground. This would activate one out of four debug functions based on the setting of one of the encoders on the side of the device. A detailed description of this functionality can be read in `disassembler/observations.txt`. This functionality allows the results of any coins thrown through the coin tester to be logged over an external serial link (at 1150 baud, 7N1 format, TTL@5V). It also revealed how the calibration functionality of the device worked, but sadly this only allows a single type of coin to be used.

So instead another script was used with the Arduino to extract data from a large sample set of coins to a PC (`serial_debugger`) where it could then be analysed and built into a new config using `config_builder.py`. This config can then be loaded onto the eeprom chip by connecting to it directly while the main microcontroller is removed, using the tools in `eeprom_editor`. By doing this it proved possible to reprogram the coin counter to eat coins of 10, 20 and 50 cents, as well as 1 euro and 2 euro coins. A small mechanical modification (widening of the input slot) was necessary to have the device accept the 50 cent, 1 euro and 2 euro coins smoothly as these are significantly thicker than normal yen coins.

With these changes made and the microcontroller placed back into its socket, the coin tester was able to successfully process euro coins.



