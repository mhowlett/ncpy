#ifndef NCPY_H
#define NCPY_H

#include <nn.h>
#include <reqrep.h>
#include <stdio.h>

// todo: allow chunk size and port to be specified on comand line.
#define CHUNK_SIZE 65536
//#define CHUNK_SIZE 128
#define PORT 9005

#define COMMAND_ID_SIZE 1

#define COMMAND_FINISHED 0
#define COMMAND_GETCHUNK 1
#define COMMAND_GETMAXCHUNK 2
#define COMMAND_FILENAME 3

#endif
