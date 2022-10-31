#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"

#define BUFSIZE 255

#undef DEBUG

Config * ReadConfig (char * file)
{
	FILE * in;
	Config * retval = (Config *) malloc (sizeof(Config));

	// DEFAULTS
	retval->min_edgewidth = -1; // auto
	retval->max_edgecount = 60; // when auto, max 60 edges
	retval->ignore_refresh = 0; // don't ignore refreshes

	in = fopen (file, "r");

	if (in == NULL)
	{
		char * error = "Error opening configfile ('";
		char * errmsg = (char *) malloc (strlen(error) + strlen(file) + 2 + 1);
		sprintf(errmsg, "%s%s')", error, file);
		perror(errmsg);
		return retval;
	};

	char buffer[BUFSIZE];

	while (fgets(buffer, BUFSIZE, in))
	{
		char option[BUFSIZE];
		char value[BUFSIZE];
		if (!sscanf(buffer, "%s %s", &option, &value))
			continue;

#ifdef DEBUG
		fprintf(stderr, "Read option: %s (%s)\n", option, value);
#endif

		if (strcmp(option, "min_edgewidth") == 0)
		{
			sscanf(value, "%d", &retval->min_edgewidth);
		}
		else if (strcmp(option, "max_edgecount") == 0)
		{
			sscanf(value, "%d", &retval->max_edgecount);
		}
		else if (strcmp(option, "ignore_refresh") == 0)
		{
			sscanf(value, "%d", &retval->ignore_refresh);
#ifdef DEBUG
			fprintf(stderr, "Ignore_refresh is %d\n", retval->ignore_refresh);
#endif
		}
		// Options for other parts of the series
		else if ((strcmp(option, "unify") == 0)
			|| (strcmp(option, "ignore") == 0))
		{
			// ignore
		}
		else
		{
			fprintf(stderr, "Unknown argument: %s - ignoring.\n", option);
		}
	}

	return retval;
}
