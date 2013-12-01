#ifndef NCPY_IO_H
#define NCPY_IO_H

int filesize(const char *path);
int readchunk(const char *path, char **result, int chunknum);
int writefile(const char *path, char *bytes, int size);

#endif
