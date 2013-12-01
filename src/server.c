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
  int fsize = filesize(path);

  if (fsize < 0) return 1;
  maxchunk = fsize/CHUNK_SIZE + ((fsize % CHUNK_SIZE == 0) ? -1 : 0);

  sprintf(addr, "tcp://*:%d", port);

  int timeout_ms = 10000;
  socket = nn_socket(AF_SP, NN_REP);
  endpoint = nn_bind(socket, addr);
  nn_setsockopt(socket, NN_SOL_SOCKET, NN_RCVTIMEO, &timeout_ms, sizeof(int));
  nn_setsockopt(socket, NN_SOL_SOCKET, NN_SNDTIMEO, &timeout_ms, sizeof(int));

  int idlecnt = 0;
  while (idlecnt < 100)
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
      const int headerlen = sizeof(int) + COMMAND_ID_SIZE;
      int id = *((int *)(cmdbuf+1));
      int chunklen = (id==fsize/CHUNK_SIZE) ? fsize % CHUNK_SIZE : CHUNK_SIZE;
      char* sendbuf = (char *)malloc(chunklen + headerlen);

      sendbuf[0] = COMMAND_GETCHUNK;
      memcpy(sendbuf + COMMAND_ID_SIZE, &id, sizeof(int)); 
      readchunk(path, &data, id); 
      memcpy(sendbuf+headerlen, data, chunklen);

      erase_line();
      printf("sending chunk: %d/%d", id+1, maxchunk+1);
      fflush(stdout);

      nn_send(socket, sendbuf, chunklen + headerlen, 0);
      free(sendbuf);

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
