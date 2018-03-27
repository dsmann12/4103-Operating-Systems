#ifndef WRAPPERS_H
#define WRAPPERS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void error_handler(const char *msg);
void usage(const char *program, const char *args);
void Close(const int fd);

#endif
