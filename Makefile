lcc=$(HOME)/gbdk/bin/lcc
bgb=$(PWD)/tools/BGB/bgb.exe

compile:
	$(lcc) -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -c -o main.o main.c
	$(lcc) -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -o main.gb main.o

run:
	$(bgb) -f main.gb
