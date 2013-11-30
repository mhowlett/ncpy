#include "server.h"
#include "utils.h"

int execute_server(int port, char* path)
{
  int rc;
  int socket;
  int endpoint;

  char cmdbuf[sizeof(int) + COMMAND_ID_SIZE];
  char chunkbuf[CHUNK_SIZE + COMMAND_ID_SIZE];
  char addr[1024];

  int maxchunk;
  char* data;
  int size = readfile(path, &data);
  if (size < 0) return 1;
  maxchunk = size/CHUNK_SIZE + ((size % CHUNK_SIZE == 0) ? -1 : 0);

  sprintf(addr, "tcp://*:%d", port);

  int timeout_ms = 1000;
  socket = nn_socket(AF_SP, NN_PAIR);
  endpoint = nn_bind(socket, addr);
  //nn_setsockopt(socket, NN_SOL_SOCKET, NN_RCVTIMEO, &timeout_ms, sizeof(int));
  //nn_setsockopt(socket, NN_SOL_SOCKET, NN_SNDTIMEO, &timeout_ms, sizeof(int));

  int idlecnt = 0;
  while (idlecnt < 10000)
  {
    rc = nn_recv(socket, cmdbuf, COMMAND_ID_SIZE + sizeof(int), 0);

    if (rc <= 0)
    {
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
      int bufsize = strlen(path) + COMMAND_ID_SIZE + 1;
      char* tmpbuf = malloc(bufsize);
      tmpbuf[0] = COMMAND_FILENAME;
      strcpy(tmpbuf + COMMAND_ID_SIZE, path);
      nn_send(socket, tmpbuf, bufsize, 0);
      free(tmpbuf);
      continue;
    }

    if (cmdbuf[0] == COMMAND_GETMAXCHUNK)
    {
      *cmdbuf = COMMAND_GETMAXCHUNK;
      *((int *)(cmdbuf + 1)) = maxchunk;
      nn_send(socket, cmdbuf, sizeof(int) + 1, 0);
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
