CC = gcc -m32
#CC = arm-linux-gnueabi-gcc
#CC=/opt/intel/compiler60/ia32/bin/icc

#CFLAGS = -g -Wall
CFLAGS = -g -Wall -fno-common
#CFLAGS = -Wall -O6 -fomit-frame-pointer -ffast-math -funroll-loops -march=pentiumpro
#CFLAGS = -g -Wall -W -pedantic -std=c99
#CFLAGS = -Os -Wall -pedantic
#CFLAGS = -Os -Wall -fomit-frame-pointer -ffast-math -march=pentiumpro
#CFLAGS=-O3 -xiMK -tpp6 -c99 -wp_ipo -g
OBJS = objs.o misc.o id_ca.o id_vh.o id_us.o \
	wl_act1.o wl_act2.o wl_act3.o wl_agent.o wl_game.o \
	wl_inter.o wl_menu.o wl_play.o wl_state.o wl_text.o wl_main.o \
	wl_debug.o vi_comm.o tables.o mapheaders.o pagemap.o
ROBJS = wl_draw.o
SOBJS = $(OBJS) $(ROBJS) vi_svga.o
XOBJS = $(OBJS) $(ROBJS) vi_xlib.o
DOBJS = $(OBJS) $(ROBJS) vi_sdl.o

#LDLIBS = -lm -wp_ipo
LDLIBS = -lm

# no sound
OBJS += sd_null.o
# sound using OSS
#OBJS += sd_oss.o fmopl.o
#CFLAGS += -D_REENTRANT
#LDLIBS += -lpthread

#CFLAGS += $(shell sdl-config --cflags)

SLDLIBS = $(LDLIBS) -lvga
XLDLIBS = $(LDLIBS) -L/usr/X11R6/lib -lX11 -lXext
DLDLIBS = $(LDLIBS) $(shell sdl-config --libs)

NASM = nasm

.SUFFIXES: .asm

#all:	swolf3d xwolf3d sdlwolf3d
all:	xwolf3d

$(SOBJS): version.h id_heads.h wl_def.h
$(XOBJS): version.h id_heads.h wl_def.h
$(DOBJS): version.h id_heads.h wl_def.h
id_ca.o: huffman.h

.asm.o:
	$(NASM) -f elf -o $@ $<

swolf3d: $(SOBJS)
	$(CC) -o swolf3d $(SOBJS) $(SLDLIBS) 

xwolf3d: $(XOBJS)
	$(CC) -o xwolf3d $(XOBJS) $(XLDLIBS)

sdlwolf3d: $(DOBJS)
	$(CC) -o sdlwolf3d $(DOBJS) $(DLDLIBS)

tables.o: tables.c

tables.c: build_tables.c
	gcc build_tables.c -o build_tables -lm
	./build_tables > tables.c

huffman.h: build_huffman.c
	gcc build_huffman.c -o build_huffman -lm
	./build_huffman > huffman.h

mapheaders.c: build_mapheaders.c
	gcc build_mapheaders.c -o build_mapheaders -lm
	./build_mapheaders > mapheaders.c

pagemap.c: build_pagemap.c
	gcc build_pagemap.c -o build_pagemap -lm
	./build_pagemap > pagemap.c

clean:
	rm -rf swolf3d xwolf3d sdlwolf3d *.o *.il build_tables tables.c \
	  build_huffman huffman.h build_mapheaders mapheaders.c

distclean: clean
	rm -rf *~ DEADJOE

forced: distclean all
	rm -rf *.o
