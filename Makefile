# Copyright (c) 2013 Matthew Howlett
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

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
