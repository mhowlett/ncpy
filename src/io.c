#include <stdio.h>

#include "io.h"
#include "ncpy.h"

int readchunk(const char *path, char **result, int chunknum)
{
  int size = 0;
  FILE *f = fopen(path, "rb");
  if (f == NULL) 
  { 
    *result = NULL;
    printf("unable to open '%s'\n", chunknum, path);
    return -1;
  } 
  fseek(f, 0, SEEK_END);
  size = ftell(f);
  int startpos = chunknum * CHUNK_SIZE;
  fseek(f, startpos, SEEK_SET);
  int toread = CHUNK_SIZE;
  if (size-startpos < CHUNK_SIZE)
  {
    toread = size-startpos;
  }
  *result = (char *)malloc(toread+1);
  if (toread != fread(*result, sizeof(char), toread, f)) 
  { 
    free(*result);
    printf("unable to read chunk #%d from '%s'\n", chunknum, path);
    return -2;
  } 
  fclose(f);
  (*result)[toread] = 0;
  return toread;
}

int filesize(const char *path)
{
  int size = 0;
  FILE *f = fopen(path, "rb");
  if (f == NULL) 
  { 
    printf("unable to determine size of '%s'\n", path);
    return -1;
  } 
  fseek(f, 0, SEEK_END);
  size = ftell(f);
  fclose(f);
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

