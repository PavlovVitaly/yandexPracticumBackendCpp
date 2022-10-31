#ifndef DOTGEN_H
#define DOTGEN_H

#include "graph.h"
#include "config.h"

void GenerateDot (FILE * dest, AnnotatedGraph * g, NodeHashTbl * nodes, Config * config);

#endif
