# ----------------------------------------------------------------------------
# Makefile for building tapp


ifndef DESTDIR
DESTDIR			   ?= /tmp/XXmanufacture/XXmachine/rootfs
endif

CFLAGS				= -Wall -O2
CC					= arm-linux-gcc
INSTALL				= install

TARGET				= led-player


all: $(TARGET)

led-player: led-player.c
	$(CC) $(CFLAGS) $< -o $@


install: $(TARGET)
	$(INSTALL) $^ $(DESTDIR)/usr/bin

clean distclean:
	rm -rf *.o $(TARGET)


# ----------------------------------------------------------------------------

.PHONY: $(PHONY) install clean distclean

# End of file
# vim: syntax=make

