#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "graph.h"
#include "readfile.h"
#include "dotgen.h"
#include "config.h"

void printUsage()
{
	fprintf(stderr, "events2dot <eventsfile>\n");
}

int main (int argc, char ** argv)
{
	NodeHashTbl * nodehash = new NodeHashTbl (255);
	GraphList g;

	if ((argc != 2) 
		|| (strcmp(argv[1], "--help") == 0)
		|| (strcmp(argv[1], "-help") == 0)
		|| (strcmp(argv[1], "-?") == 0)
		|| (strcmp(argv[1], "-h") == 0))
	{
		printUsage();
		exit(0);
	}

	Config * config;
	config = ReadConfig ("pathalizer.conf");

	g = getGraphFromFile(argv[1], nodehash, config);

	AnnotatedGraph * ag = summarize(g, config);

	GenerateDot (stdout, ag, nodehash, config);

	return 0;
}
