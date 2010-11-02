#ifndef __POLALIB_H_
#define __POLALIB_H_

#define _GNU_SOURCE

#include <dirent.h>
#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define O_RDONLY 00
#define O_WRONLY 01
#define O_RDWR   02
#define OPEN_FLAGS (O_RDONLY | O_WRONLY | O_RDWR)


int open(const char* pathname, int flags, mode_t mode);
int non_inter_open(const char* pathname, int flags, mode_t mode);
int inter_open(const char* pathname, int flags, mode_t mode);

DIR *opendir(const char* name);
DIR *non_inter_opendir(const char* name);
DIR *inter_opendir(const char* name);


#endif
