# nanomsgx makefile
# Copyright (C) 2011 Matt Howlett <matt dot howlett at gmail dot com>
# This file is released under an MIT license, see the LICENSE.txt file.

NANOMSG_PATH=/opt/nanomsg/src
NANOMSG_LIB_PATH=/usr/local/lib

all: ncpy

ncpy: objs
	gcc -o ncpy src/ncpy.o src/client.o src/server.o src/io.o src/utils.o -lnanomsg

objs: src/ncpy.c src/io.c src/io.h src/server.c src/server.h src/client.c src/client.h src/utils.c src/utils.h
	gcc -I$(NANOMSG_PATH) -w -c src/ncpy.c -o src/ncpy.o
	gcc -I$(NANOMSG_PATH) -w -c src/io.c -o src/io.o
	gcc -I$(NANOMSG_PATH) -w -c src/server.c -o src/server.o
	gcc -I$(NANOMSG_PATH) -w -c src/client.c -o src/client.o
	gcc -I$(NANOMSG_PATH) -w -c src/utils.c -o src/utils.o

clean:
	rm -f src/*.o
	rm -f src/*~
	rm -f *~
	rm -f ncpy
