#include <stdio.h>

#include "io.h"

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

