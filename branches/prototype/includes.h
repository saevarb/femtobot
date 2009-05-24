
#ifndef __INCLUDES_H_
#define __INCLUDES_H_
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <pthread.h>


/* Defines must be before includes of
 * custom header files to make sure they
 * can be used in them. */
#define MAX_MESSAGE_LENGTH 512
#define MAX_VARIABLE_LENGTH 64

#include "botstructs.h"
#include "prototypes.h"

#endif
