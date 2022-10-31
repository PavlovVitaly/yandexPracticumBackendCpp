#include <assert.h>
#include "graph.h"

#define BUFSIZE 100
#undef DEBUG

/* a pointer to this struct * is passed on to FindTreshold while walking */
struct findtreshold_arg
{
        int n_edges;
        int current_treshold;
};

void CountEdges (void * content, void * arg)
{
        findtreshold_arg * args = (findtreshold_arg *) arg;
        // TODO wat als edge->next != null? Juist!
        AnnotatedEdge * current = (AnnotatedEdge *)content;
        while (current != NULL)
        {
                if (current->n_taken > args->current_treshold)
                        args->n_edges++;
                current = current->next;
        }
}

int FindTreshold(BinaryTree * tree_root, int max_edgecount)
{
	static findtreshold_arg * args = (findtreshold_arg*) malloc (sizeof(findtreshold_arg));
	args->n_edges=-1;
	args->current_treshold=0;

#ifdef DEBUG
	fprintf(stderr, "  Finding treshold. max_edgecount: %d\n", max_edgecount);
#endif
	
	while ((args->n_edges > max_edgecount) || (args->n_edges == -1))
	{
		//AnnotatedEdge * current_edge = start_edge;
		args->n_edges = 0;

		tree_root->walk(CountEdges, args);

#ifdef DEBUG
		fprintf(stderr, "  With treshold %d, found %d edges.\n", current_treshold, edges);
#endif

		args->current_treshold++;
	}
	return args->current_treshold-1;
}

struct printedge_arg
{
        int min_edgewidth;
};

void PrintNode (void * content, void * arg)
{
	Node * node  = (Node *)content;
	FILE * dest  = (FILE *)arg;
	char * shape = "ellipse";

	if (!node->used)
		return;
	if ((node->start > 0) && (node->end > 0))
	{
		shape = "octagon";
	} 
	else if (node->start > 0)
	{
		shape = "box";
	}
	else if (node->end > 0)
	{
		shape = "diamond";
	}
	fprintf (dest, "\"%s\" [shape=%s];\n",
			node->name,
			shape);
}
void PrintEdge (void * content, void * arg)
{
        printedge_arg * args = (printedge_arg*) arg;
        AnnotatedEdge * current = (AnnotatedEdge *)content;

        while (current != NULL)
        {
                if (current->n_taken > args->min_edgewidth)
                {
                        printf("\"%s\" -> \"%s\"[label=%d,color=\"0,0,%f\"];\n", 
                                        current->from->name,
                                        current->to->name,
                                        current->n_taken, 
					1.0-current->n_taken/60.0);
                         current->from->used = true;
                         current->to->used = true;
                }
                current = current->next;
        }
}


	
void GenerateDot (FILE * dest, AnnotatedGraph * g, NodeHashTbl * nodehash, Config * config)
{
	fprintf(dest, "digraph site_usage {\n");
	//fprintf(dest, "concentrate=true\n");
	fprintf(dest, "size=\"7,10\"\n");
	fprintf(dest, "page=\"8.5,11\"\n");
	fprintf(dest, "rotate=90\n");
	fprintf(dest, "center=\"\";\n");
	fprintf(dest, "node[width=.25,hight=.375,fontsize=9]\n");

	printedge_arg * args = (printedge_arg*) malloc (sizeof(printedge_arg));

	if (config->min_edgewidth < 0)
	{
		args->min_edgewidth = FindTreshold(g->edgetree, config->max_edgecount);
		fprintf(stderr, "  Chose treshold: %d\n", args->min_edgewidth);
	} else {
		args->min_edgewidth = config->min_edgewidth;
	}

	g->edgetree->walk (PrintEdge, args);
	nodehash->walk (PrintNode, dest);

	/* TODO walk nodes */

	fprintf(dest, "}\n");
}
