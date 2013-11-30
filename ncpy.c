#include <nn.h>
#include <reqrep.h>
#include <stdio.h>

// todo: allow chunk size and port to be specified on comand line.
#define CHUNK_SIZE 65536
#define PORT 9005

#define COMMAND_FINISHED 0
#define COMMAND_GETCHUNK 1
#define COMMAND_GETMAXCHUNK 2
#define COMMAND_FILENAME 3

int readfile(const char *path, char **result) 
{ 
  int size = 0;
  FILE *f = fopen(path, "rb");
  if (f == NULL) 
  { 
    *result = NULL;
    printf("unable to open file '%s'\n", path);
    return -1;
  } 
  fseek(f, 0, SEEK_END);
  size = ftell(f);
  fseek(f, 0, SEEK_SET);
  *result = (char *)malloc(size+1);
  if (size != fread(*result, sizeof(char), size, f)) 
  { 
    free(*result);
    printf("unable to read file '%s'\n", path);
    return -2;
  } 
  fclose(f);
  (*result)[size] = 0;
  return size;
}

int writefile(const char *path, char *bytes, int size)
{
  FILE *f = fopen(path, "wb");
  int fd = fileno(f);

  if (f == NULL)
  {
    printf("unable to open file '%s' for writing.\n", path);
    return -1;
  }

  int num = write(fd, bytes, size);

  fclose(f);

  return 0;
}

void erase_line()
{
  printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
}

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

int display_usage()
{
  printf("ncpy -c address\n");
  printf("ncpy -s filename\n");
  return 1;
}

int main(int argc, char* argv[])
{
  if (argc == 3 && strcmp(argv[1], "-c") == 0)
  {
    return execute_client(argv[2]);
  }
  
  if (argc == 3 && strcmp(argv[1], "-s") == 0)
  {
    return execute_server(PORT, argv[2]);
  }

  return display_usage();
}
