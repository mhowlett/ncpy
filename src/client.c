/*
 Copyright (c) 2013 Matthew Howlett

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal 
 in the Software without restriction, including without limitation the rights 
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/


#include "client.h"
#include "utils.h"

int send_cmd(int socket, char command, int data)
{
  const int bufsize = sizeof(int) + 1;
  char buf[bufsize];
  buf[0] = command;
  *((int *)(buf+1)) = data;
  nn_send(socket, buf, bufsize, 0);

  return 0;
}
 
int recv_cmd_string(int socket, char command, char** data)
{ 
  int rc; 
  char* buf;
  rc = nn_recv(socket, &buf, NN_MSG, 0);
  if (buf[0] != command)
  {
    printf("network error: receiving command type %d", command);
    return -1;
  }

  int len = strlen(buf+1);
  *data = (char *)malloc(len * sizeof(char) + 1);
  strcpy(*data, buf+1);

  nn_freemsg(buf);

  return 0;
}

int recv_cmd_int(int socket, char command, int* data)
{
  int rc;
  int bufsize = sizeof(int) + 1;
  char buf[bufsize];
  rc == nn_recv(socket, buf, bufsize, 0);
  if (buf[0] != command)
  {
    printf("network error: receiving command type %d", command);
    return -1;
  }
  *data = *((int *)(buf+1));

  return 0;
}

int recv_cmd_chunk(int socket, int i, char* data)
{
  const int headerlen = COMMAND_ID_SIZE + sizeof(int);
  const int buflen = CHUNK_SIZE + headerlen;
  char buf[buflen];
  int rc;

  rc = nn_recv(socket, buf, buflen, 0);
  if (rc < 0)
  {
    return -1;
  }
  int receivedlen = rc;

  if (buf[0] != COMMAND_GETCHUNK)
  {
    return -2;
  }

  int goti = *((int *)(buf+1));
  if (goti != i)
  {
    return -3;
  }

  int chunklen = receivedlen - headerlen;
  memcpy(data, buf + headerlen, chunklen);

  return chunklen;
}

int get_filename_and_numchunks(int socket, char** filename, int* maxchunk)
{
  // this part is not robust against network outages.

  send_cmd(socket, COMMAND_FILENAME, 0);
  recv_cmd_string(socket, COMMAND_FILENAME, filename);

  send_cmd(socket, COMMAND_GETMAXCHUNK, 0);
  recv_cmd_int(socket, COMMAND_GETMAXCHUNK, maxchunk);
}

int execute_client(char* a)
{
  int socket;
  int endpoint;
  int rc;
  char addr[1024];

  char chunkbuf[CHUNK_SIZE];
  
  sprintf(addr, "tcp://%s:%d", a, PORT);

  socket = nn_socket(AF_SP, NN_REQ);
  endpoint = nn_connect(socket, addr);

  char* filename;
  int maxchunk;
  get_filename_and_numchunks(socket, &filename, &maxchunk);

  int bufsize = CHUNK_SIZE*(maxchunk+1);
  char *data = (char *)malloc(bufsize);

  int timeout_ms = 10000;
  nn_setsockopt(socket, NN_SOL_SOCKET, NN_RCVTIMEO, &timeout_ms, sizeof(int));
  nn_setsockopt(socket, NN_SOL_SOCKET, NN_SNDTIMEO, &timeout_ms, sizeof(int));

  int i;
  for (i=0; i<=maxchunk; ++i)
  {
    erase_line();
    printf("receiving chunk %d/%d", i+1, maxchunk+1);
    fflush(stdout);

    send_cmd(socket, COMMAND_GETCHUNK, i);
    rc = -1;
    int errorcount = 0;
    while (rc < 0)
    {
      rc = recv_cmd_chunk(socket, i, chunkbuf);
      if (rc < 0)
      {
	errorcount += 1;
	erase_line();
	printf("receiving chunk %d/%d [recv error x%d]", i+1, maxchunk+1, errorcount);
	fflush(stdout);

	nn_close(socket);
	nn_socket(AF_SP, NN_REQ);
	nn_connect(socket, addr);
        nn_setsockopt(socket, NN_SOL_SOCKET, NN_RCVTIMEO, &timeout_ms, sizeof(int));
        nn_setsockopt(socket, NN_SOL_SOCKET, NN_SNDTIMEO, &timeout_ms, sizeof(int));	
        send_cmd(socket, COMMAND_GETCHUNK, i);
      }
    }

    if (rc > 0)
    {
      appendtofile(filename, chunkbuf, rc);
    }
  }

  send_cmd(socket, COMMAND_FINISHED, 0);

  erase_line();
  printf ("%s: receive complete\n", filename);

  free(filename);

  return rc;
}
