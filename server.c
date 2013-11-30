#include "server.h"
#include "utils.h"

int execute_server(int port, char* path)
{
  int rc;
  int socket;
  int endpoint;

  char cmdbuf[5];
  char addr[1024];

  int maxchunk;
  char* data;
  int size = readfile(path, &data);
  if (size < 0) return 1;
  maxchunk = size/CHUNK_SIZE + ((size % CHUNK_SIZE == 0) ? -1 : 0);

  sprintf(addr, "tcp://*:%d", port);
  socket = nn_socket(AF_SP, NN_REP);
  endpoint = nn_bind(socket, addr);

  int idlecnt = 0;
  while (idlecnt < 10000)
  {
    rc = nn_recv(socket, cmdbuf, 5, NN_DONTWAIT);

    if (rc <= 0)
    {
      usleep(1000);
      idlecnt += 1;
      continue;
    }
    idlecnt = 0;

    if (rc != 5)
    {
      printf("netwok error: incorrect command length.\n");
      break;
    }
    
    if (cmdbuf[0] == COMMAND_FINISHED)
    {
      erase_line();
      printf("%s: send complete\n", path);
      break;
    }

    if (cmdbuf[0] == COMMAND_FILENAME)
    {
      nn_send(socket, path, strlen(path)+1, 0);
      continue;
    }

    if (cmdbuf[0] == COMMAND_GETMAXCHUNK)
    {
      *((int *)cmdbuf) = maxchunk;
      nn_send(socket, cmdbuf, sizeof(int), 0);
      continue;
    }

    if (cmdbuf[0] == COMMAND_GETCHUNK)
    {
      int id = *((int *)(cmdbuf+1));
      int chunklen = (id == size/CHUNK_SIZE) ? size % CHUNK_SIZE : CHUNK_SIZE;
      erase_line();
      printf("sending chunk: %d/%d", id+1, maxchunk+1);
      fflush(stdout);
      nn_send(socket, data + id * CHUNK_SIZE, chunklen, 0);
      continue;
    }

    printf("network error (2)\n");
    break;
  }

  if (idlecnt >= 10)
  {
    printf("idle too long, giving up.\n");
  }

  free(data);
  return 0;
}
