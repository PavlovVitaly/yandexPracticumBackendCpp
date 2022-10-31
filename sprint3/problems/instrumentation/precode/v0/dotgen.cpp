#include <assert.h>
#include "graph.h"

#define BUFSIZE 100
#undef DEBUG

int FindTreshold(AnnotatedEdge * start_edge, int max_edgecount)
{
	int edges=-1;
	int current_treshold=1;

#ifdef DEBUG
	fprintf(stderr, "  Finding treshold. max_edgecount: %d\n", max_edgecount);
#endif
	
	while ((edges > max_edgecount) || (edges == -1))
	{
		AnnotatedEdge * current_edge = start_edge;
		edges = 0;

		while (current_edge != NULL)
		{
			if (current_edge->n_taken > current_treshold)
				edges++;
			current_edge = current_edge->next;
		}

#ifdef DEBUG
		fprintf(stderr, "  With treshold %d, found %d edges.\n", current_treshold, edges);
#endif

		current_treshold++;
	}
	return current_treshold-1;
}
	
void GenerateDot (FILE * dest, AnnotatedGraph * g, NodeList nodes, Config * config)
{
	fprintf(dest, "digraph site_usage {\n");
	//fprintf(dest, "concentrate=true\n");
	fprintf(dest, "size=\"7,10\"\n");
	fprintf(dest, "page=\"8.5,11\"\n");
	fprintf(dest, "rotate=90\n");
	fprintf(dest, "center=\"\";\n");
	fprintf(dest, "node[width=.25,hight=.375,fontsize=9]\n");

	int min_edgewidth;
	if (config->min_edgewidth < 0)
	{
		min_edgewidth = FindTreshold(g->edges, config->max_edgecount);
		fprintf(stderr, "  Chose treshold: %d\n", min_edgewidth);
	} else {
		min_edgewidth = config->min_edgewidth;
	}

	AnnotatedEdge * current_edge = g->edges;

	while (current_edge != NULL)
	{
		if (current_edge->n_taken > min_edgewidth)
		{
			fprintf(dest, "\"%s\" -> \"%s\"[label=%d,color=\"0,0,%f\"];\n", 
					current_edge->from->name,
					current_edge->to->name,
					current_edge->n_taken, 1.0-current_edge->n_taken/60.0);
			 current_edge->from->used = true;
			 current_edge->to->used = true;
		}

		current_edge = current_edge->next;
	}

	NodeListNode * current_nodelistnode = nodes;

	while (current_nodelistnode != NULL)
	{
		char * shape;
		shape = "ellipse";

		if (!current_nodelistnode->node->used)
		{
			current_nodelistnode = current_nodelistnode->next;
			continue;
		}

		if ((current_nodelistnode->node->start > 0)
				&& (current_nodelistnode->node->end > 0))
		{
			shape = "octagon";
		}
		else if (current_nodelistnode->node->start > 0)
		{
			shape = "box";
		}
		else if (current_nodelistnode->node->end > 0)
		{
			// sprintf(color, "\"0,0,0.5\"", current_nodelistnode->node->end/10);
			shape = "diamond";
		}
		
		fprintf(dest, "\"%s\" [shape=%s];\n", 
				current_nodelistnode->node->name,
				shape);

		current_nodelistnode = current_nodelistnode->next;
	}

	fprintf(dest, "}\n");
}
