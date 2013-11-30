# nanomsgx makefile
# Copyright (C) 2011 Matt Howlett <matt dot howlett at gmail dot com>
# This file is released under an MIT license, see the LICENSE.txt file.

NANOMSG_PATH=/opt/nanomsg/src
NANOMSG_LIB_PATH=/usr/local/lib

all: ncpy

ncpy: ncpy.o
	gcc -o ncpy ncpy.o -lnanomsg

ncpy.o: ncpy.c
	gcc -I$(NANOMSG_PATH) -w -c ncpy.c

clean:
	rm -rf *.o
	rm -rf *~
	rm -rf ncpy
