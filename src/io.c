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

int appendtofile(const char *path, char *bytes, int size)
{
  FILE *f = fopen(path, "ab");
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

