#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "graph.h"
#include "config.h"

#define BUFSIZE 255

GraphList getGraphFromFile (char * file, NodeHashTbl * nodelist, Config * config);
