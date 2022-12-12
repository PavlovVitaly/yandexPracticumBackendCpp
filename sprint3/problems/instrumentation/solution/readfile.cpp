#include "readfile.h"

#undef DEBUG

GraphList getGraphFromFile (char * file, NodeHashTbl * nodehash, Config * config)
{
	FILE * in;
	GraphListNode * current_graphlistnode = NULL;

	in = fopen (file, "r");

	if (in == NULL)
	{
		char * error = "Error opening file with events ('";
		char * errmsg = (char *) malloc (strlen(error) + strlen(file) + 2 + 1);
		sprintf(errmsg, "%s%s')", error, file);
		perror(errmsg);
		exit(0);
	};

	char buffer[BUFSIZE];
	char session[BUFSIZE];
	int timestamp;
	char name[BUFSIZE];

	char * current_session = (char *) malloc (1); // so we can free it
	Node * last_node = NULL;
	Node * current_node = NULL;

#ifdef DEBUG
	fprintf(stderr, "Ignoring refreshes: %d", config->ignore_refresh);
#endif

	// while (fscanf(in, "%s\t%d\t%s\n", &session, &timestamp, &name) > 0)
	while (fgets (buffer, BUFSIZE, in))
	{
		if (!sscanf(buffer, "%s\t%d\t%s\n", &session, &timestamp, &name))
			break;

		last_node = current_node;

#ifdef DEBUG
		fprintf(stderr, "Session %s, node %s\n", session, name);
#endif
		current_node = getNode(name, nodehash);

		if (strcmp(session, current_session) != 0)
		{
			free (current_session);
			current_session = strdup(session);
			// TODO maybe check for graphs without edges?
			current_graphlistnode = newGraphListNode(current_graphlistnode, current_node);
		}
		else
		{
			if ((!config->ignore_refresh) // if false, just add the edge
					|| (strcmp(last_node->name, current_node->name) != 0))
			{
				addEdge(current_graphlistnode->graph, last_node, current_node);
			}
		}
	}

	return current_graphlistnode;
}
