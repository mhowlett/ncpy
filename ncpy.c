#include <nn.h>
#include <reqrep.h>
#include <stdio.h>

#define COMMAND_FINISHED 0
#define COMMAND_GETCHUNK 1
#define COMMAND_GETMAXCHUNK 2
#define COMMAND_FILENAME 3

#define CHUNK_SIZE 128

#define PORT "9005"

int readfile(const char *filename, char **result) 
{ 
  int size = 0;
  FILE *f = fopen(filename, "rb");
  if (f == NULL) 
  { 
    *result = NULL;
    printf("unable to open file %s\n", filename);
    return -1;
  } 
  fseek(f, 0, SEEK_END);
  size = ftell(f);
  fseek(f, 0, SEEK_SET);
  *result = (char *)malloc(size+1);
  if (size != fread(*result, sizeof(char), size, f)) 
  { 
    free(*result);
    printf("unable to read file %s\n", filename);
    return -2;
  } 
  fclose(f);
  (*result)[size] = 0;
  return size;
}

int writefile(const char *filename, char *bytes, int size)
{
  FILE *f = fopen(filename, "wb");
  int fd = fileno(f);

  if (f == NULL)
  {
    printf("unable to open file %s for write.\n", filename);
    return -1;
  }

  int num = write(fd, bytes, size);
  printf( "wrote %d bytes\n", num);

  fclose(f);

  return 0;
}

int execute_client(char* a)
{
  char addr[1024];
  char buf[CHUNK_SIZE];
  int socket;
  int endpoint;
  sprintf(addr, "tcp://%s:%s", a, PORT);

  socket = nn_socket(AF_SP, NN_REQ);
  endpoint = nn_connect(socket, addr);

  buf[0] = COMMAND_FILENAME;
  nn_send(socket, buf, sizeof(int) + 1, 0);
  

  buf[0] = COMMAND_GETMAXCHUNK;
  nn_send(socket, buf, sizeof(int) + 1, 0);
  int rc = nn_recv(socket, buf, CHUNK_SIZE, 0);
  
  int maxchunk = *((int *)buf);
  int bufsize = CHUNK_SIZE*(maxchunk+1);
  char *bytes = (char *)malloc(bufsize);

  printf ("%d\n", maxchunk);

  int i;
  for (i =0; i<=maxchunk; ++i)
  {
    printf("getting chunk #%d\n", i);

    buf[0] = COMMAND_GETCHUNK;
    *((int *)(buf+1)) = i;

    nn_send(socket, buf, sizeof(int) + 1, 0);

    rc = nn_recv(socket, buf, CHUNK_SIZE, 0);
    if (rc < 0)
    {
      printf("network error\n");
      break;
    }

    memcpy(bytes + CHUNK_SIZE*i, buf, rc);
  }

  buf[0] = COMMAND_FINISHED;
  nn_send(socket, buf, sizeof(int) + 1, 0);

  int size = rc + maxchunk*CHUNK_SIZE;
  return writefile("out.bin", bytes, size);
}

int execute_server(char* port, char* path)
{
  char buf[1024];
  char addr[1024];
  int socket;
  int endpoint;

  char* bytes;
  int size =readfile(path, &bytes);
  if (size < 0)
  {
    return 1;
  }
  printf("filee #bytes: %d\n", size);

  socket = nn_socket(AF_SP, NN_REP);

  sprintf(addr, "tcp://*:%s", port);
  endpoint = nn_bind(socket, addr);

  int idlecnt = 0;
  while (idlecnt < 10000)
  {
    int rc =  nn_recv(socket, buf, 1024, NN_DONTWAIT);

    if (rc <= 0)
    {
      usleep(1000);
      idlecnt += 1;
      continue;
    }
    idlecnt = 0;

    if (rc != 5)
    {
      printf("network error (1)\n");
    }
    
    if (buf[0] == COMMAND_FINISHED)
    {
      printf("finished\n");
      break;
    }

    if (buf[0] == COMMAND_GETMAXCHUNK)
    {
      *((int *)buf) = size / CHUNK_SIZE + ((size % CHUNK_SIZE == 0) ? -1 : 0);
      nn_send(socket, buf, sizeof(int), 0);
      continue;
    }

    if (buf[0] == COMMAND_GETCHUNK)
    {
      int id = *((int *)(buf+1));
      int len = (id == size / CHUNK_SIZE) ? size % CHUNK_SIZE : CHUNK_SIZE;
      printf("%d : %d\n", id, len);
      nn_send(socket, bytes + id * CHUNK_SIZE, len, 0);
      continue;
    }

    printf("network error (2)\n");
    break;
  }

  if (idlecnt >= 10)
  {
    printf("idle too long, giving up.\n");
  }

  free(bytes);
  return 0;
}

int display_usage()
{
  printf("ncpy -c sourceaddress\n");
  printf("ncpy -s port filepath\n");
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
