# nanomsgx makefile
# Copyright (C) 2011 Matt Howlett <matt dot howlett at gmail dot com>
# This file is released under an MIT license, see the LICENSE.txt file.

NANOMSG_PATH=/opt/nanomsg/src
NANOMSG_LIB_PATH=/usr/local/lib

all: ncpy

ncpy: objs
	gcc -o ncpy ncpy.o client.o server.o io.o utils.o -lnanomsg

objs: ncpy.c io.c io.h server.c server.h client.c client.h utils.c utils.h
	gcc -I$(NANOMSG_PATH) -w -c ncpy.c
	gcc -I$(NANOMSG_PATH) -w -c io.c
	gcc -I$(NANOMSG_PATH) -w -c server.c
	gcc -I$(NANOMSG_PATH) -w -c client.c
	gcc -I$(NANOMSG_PATH) -w -c utils.c

clean:
	rm -rf *.o
	rm -rf *~
	rm -rf ncpy
