#ifndef __POLASH_H_
#define __POLASH_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define nbargs 20

int setbin();
char** parser(char* tmp);


#endif
