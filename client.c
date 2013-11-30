#include "client.h"
#include "utils.h"

int execute_client(char* a)
{
  int socket;
  int endpoint;
  int rc;

  char* filename;

  char addr[1024];
  char commandbuf[sizeof(int) + 1];
  char chunkbuf[CHUNK_SIZE];

  sprintf(addr, "tcp://%s:%d", a, PORT);

  socket = nn_socket(AF_SP, NN_REQ);
  endpoint = nn_connect(socket, addr);

  commandbuf[0] = COMMAND_FILENAME;
  nn_send(socket, commandbuf, sizeof(int) + 1, 0);
  rc = nn_recv(socket, &filename, NN_MSG, 0);

  commandbuf[0] = COMMAND_GETMAXCHUNK;
  nn_send(socket, commandbuf, sizeof(int) + 1, 0);
  rc = nn_recv(socket, commandbuf, CHUNK_SIZE, 0);
  
  int maxchunk = *((int *)commandbuf);
  int bufsize = CHUNK_SIZE*(maxchunk+1);
  char *data = (char *)malloc(bufsize);

  int i;
  for (i=0; i<=maxchunk; ++i)
  {
    erase_line();
    printf("receiving chunk %d/%d", i+1, maxchunk+1);
    fflush(stdout);

    commandbuf[0] = COMMAND_GETCHUNK;
    *((int *)(commandbuf+1)) = i;
    nn_send(socket, commandbuf, sizeof(int) + 1, 0);

    rc = nn_recv(socket, chunkbuf, CHUNK_SIZE, 0);
    if (rc < 0)
    {
      printf("network error\n");
      break;
    }

    memcpy(data + CHUNK_SIZE*i, chunkbuf, rc);
  }

  commandbuf[0] = COMMAND_FINISHED;
  nn_send(socket, commandbuf, sizeof(int) + 1, 0);

  int size = rc + maxchunk*CHUNK_SIZE;
  rc = writefile(filename, data, size);

  erase_line();
  printf ("%s: receive complete\n", filename);

  nn_freemsg(filename);

  return rc;
}
