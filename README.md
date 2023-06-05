# X16-test-audio-memory
 Simple test program: writing to OPM and VERA PSG registers, vblank interrupt. It (makefile) assumes you have `cc65` and X16 emulator (`x16emu.exe`) in your PATH. It automatically calls the emulator and launches the PRG file. 

Also I modified the bank RAM linker file a bit and demostrate how dummy function indeed ends up in `B3.BIN` (function isn't used, maybe later I will add demonstration of executing the code both from main RAM and banked RAM without any conflicts; to see it inside open file in hex editor to see that it's not empty like other `Bx.BIN` files). If I succeed in placing the code and data (dynamically too) in banked RAM, I will write a **proper** tracker for Commander X16 (since it's really overpowered audio-wise and ZSM is just a register dump which is bloat and not fun).