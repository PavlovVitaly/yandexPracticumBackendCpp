#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "graph.h"
#include "readfile.h"
#include "dotgen.h"
#include "config.h"

NodeListNode * nodelist = NULL;

void printUsage()
{
	fprintf(stderr, "events2dot <eventsfile>\n");
}

int main (int argc, char ** argv)
{
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

	g = getGraphFromFile(argv[1], nodelist, config);


	AnnotatedGraph * ag = summarize(g, config);

	GenerateDot (stdout, ag, nodelist, config);

	return 0;
}
