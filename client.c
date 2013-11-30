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
    printf("network error\n");
    return -1;
  }
  int receivedlen = rc;

  if (buf[0] != COMMAND_GETCHUNK)
  {
    printf("network error: receiving chunk #%d", i);
    return -1;
  }

  int goti = *((int *)(buf+1));
  if (goti != i)
  {
    printf("network error: unexpected chunk. got %d. expected %d.", goti, i);
    return -1;
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
    while (rc < 0)
    {
      rc = recv_cmd_chunk(socket, i, chunkbuf);
      if (rc < 0)
      {
	printf("shutting down endpoint\n");
	nn_shutdown(socket, endpoint);
	printf("closing socket\n");
	nn_close(socket);
	printf("reopening socket %s\n", addr);
	socket = nn_socket(AF_SP, NN_REQ);
	printf("connecting socket\n");
	endpoint = nn_connect(socket, addr);
        nn_setsockopt(socket, NN_SOL_SOCKET, NN_RCVTIMEO, &timeout_ms, sizeof(int));
        nn_setsockopt(socket, NN_SOL_SOCKET, NN_SNDTIMEO, &timeout_ms, sizeof(int));

      }
    }

    memcpy(data + CHUNK_SIZE*i, chunkbuf, rc);
  }

  send_cmd(socket, COMMAND_FINISHED, 0);

  int size = rc + maxchunk*CHUNK_SIZE;
  rc = writefile(filename, data, size);

  erase_line();
  printf ("%s: receive complete\n", filename);

  free(filename);

  return rc;
}
