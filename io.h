#ifndef NCPY_IO_H
#define NCPY_IO_H

int readfile(const char *path, char **result);
int writefile(const char *path, char *bytes, int size);

#endif
