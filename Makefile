SRC=$(wildcard *.c)

all: test.prg

test.prg: $(SRC)
	cl65 -t cx16 -Osir -C cx16-my-bank-config.cfg -o test.prg $(SRC)
	rm -f *.o
	x16emu -prg $(CURDIR)/test.prg -run -quality nearest -scale 2
	
clean:
	rm -f *.prg *.o *.BIN
