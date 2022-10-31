#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "graph.h"

Node * newNode (char * name)
{
	Node * retval = (Node *) malloc (sizeof(Node));
	retval->name = strdup(name);
	retval->start = 0;
	retval->end = 0;
	retval->used = false;
	return retval;
}

/* remove bad characters from names, should move somewhere else probably */
void FixName (char * name)
{
	// Node names may not end with '\' or '/'
	while ((name[strlen(name)-1] == '\\')
		|| (name[strlen(name)-1] == '/'))
	{
		name[strlen(name)-1] = '\0';
	}
}


Node * getNode (char * name, NodeList &nodelist)
{
	NodeListNode * current_nodelistnode = nodelist;

	FixName(name);

	if (nodelist == NULL)
	{
		NodeListNode * new_nodelist = (NodeListNode *) malloc (sizeof(NodeListNode)); 
		Node * new_node = newNode(name);
		new_nodelist->node = new_node;
		new_nodelist->next = NULL;
		nodelist = new_nodelist;
		return new_node;
	}

	while (current_nodelistnode != NULL)
	{
		if (strcmp(name, current_nodelistnode->node->name) == 0)
		{
			return current_nodelistnode->node;
		}
		current_nodelistnode = current_nodelistnode->next;
	}

	NodeListNode * new_nodelist = (NodeListNode *) malloc (sizeof(NodeListNode)); 
	Node * new_node = newNode(name);
	new_nodelist->node = new_node;
	new_nodelist->next = nodelist;
	nodelist = new_nodelist;
	return new_node;
}

GraphListNode * newGraphListNode (GraphListNode * next, Node * start)
{
	assert (start != NULL);

	Graph * new_graph = (Graph *) malloc (sizeof(Graph));
	new_graph->name = "";
	new_graph->start = start;
	new_graph->edges = NULL;

	GraphListNode * retval = (GraphListNode *) malloc (sizeof(GraphListNode));
	retval->next = next;
	retval->graph = new_graph;

	assert (retval->graph != NULL);
	
	return retval;
}

Edge * newEdge (Node * from, Node * to, Edge * next = NULL)
{
	Edge * retval = (Edge *) malloc (sizeof(Edge));

	assert (from->name != NULL);
	assert (to->name != NULL);

	retval->from = from;
	retval->to = to;
	retval->next = next;

	return retval;
}


void addEdge (Graph * g, Node * from, Node * to)
{
	Edge * current_edge = g->edges;

	if (current_edge == NULL)
	{
		g->edges = newEdge(from, to);
		return;
	}

	while (current_edge->next != NULL)
	{
		current_edge = current_edge->next;
	}

	current_edge->next = newEdge(from, to);
}

AnnotatedEdge * newAnnotatedEdge (Edge * e, AnnotatedEdge * next = NULL)
{
	AnnotatedEdge * retval = (AnnotatedEdge *) malloc (sizeof(AnnotatedEdge));
	retval->from = e->from;
	retval->to = e->to;
	retval->next = next;
	retval->n_taken = 1;

	return retval;
}

// Returns 0 if equal names
bool CompareNodes (Node * a, Node * b)
{
	assert (a->name != NULL);
	assert (b->name != NULL);
	return strcmp (a->name, b->name);
}

// Returns 0 if equal
bool CompareEdges (Edge * a, AnnotatedEdge * b)
{
	return (CompareNodes (a->from, b->from) 
			|| CompareNodes (a->to, b->to));
}

void addAnnotatedEdge(AnnotatedGraph * g, Edge * edge)
{
	AnnotatedEdge * last_edge = NULL;
	AnnotatedEdge * current_edge = g->edges;

	if (current_edge == NULL)
	{
		g->edges = newAnnotatedEdge(edge);
		return;
	}

	while (current_edge != NULL)
	{
		if (CompareEdges(edge, current_edge) == 0)
		{
			current_edge->n_taken++;
			return;
		}
		last_edge = current_edge;
		current_edge = current_edge->next;
	}
	last_edge->next = newAnnotatedEdge(edge);
}

AnnotatedGraph * summarize (GraphList g, Config * config)
{
	AnnotatedGraph * retval = (AnnotatedGraph *) malloc (sizeof(AnnotatedGraph));
	int count = 1;

	retval->edges = NULL;

	GraphListNode * current_graphlistnode = g;

	while (current_graphlistnode != NULL)
	{
		assert (current_graphlistnode->graph != NULL);

		current_graphlistnode->graph->start->start++;

		Edge * current_edge = current_graphlistnode->graph->edges;
		Node * last_node = NULL;

		if (current_edge == NULL)
		{
			last_node = current_graphlistnode->graph->start;
		}

		while (current_edge != NULL)
		{
			last_node = current_edge->to;
			addAnnotatedEdge(retval, current_edge);
			current_edge = current_edge->next;
		}

		last_node->end++;

		current_graphlistnode = current_graphlistnode->next;
	}
	return retval;
}
