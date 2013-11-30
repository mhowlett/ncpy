# nanomsgx makefile
# Copyright (C) 2011 Matt Howlett <matt dot howlett at gmail dot com>
# This file is released under an MIT license, see the LICENSE.txt file.

NANOMSG_PATH=/opt/nanomsg/src
NANOMSG_LIB_PATH=/usr/local/lib

all: ncpy

ncpy: objs
	gcc -o ncpy ncpy.o -lnanomsg

objs: ncpy.c io.c io.h
	gcc -I$(NANOMSG_PATH) -w -c ncpy.c
	gcc -w -c io.c

clean:
	rm -rf *.o
	rm -rf *~
	rm -rf ncpy
