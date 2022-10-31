#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include "config.h"

#define N_PAGES 50

struct Node
{
	char * name;
	int start;
	int end;
	int used;
};

struct NodeListNode
{
	Node * node;
	NodeListNode * next;
};

struct Edge
{
	Node * from;
	Node * to;
	Edge * next;
};

struct AnnotatedEdge 
{
	Node * from;
	Node * to;
	AnnotatedEdge * next;
	int n_taken;
};

struct Graph
{
	char * name;
	Node * start;
	Edge * edges;
};

struct AnnotatedGraph 
{
	AnnotatedEdge * edges;
};

struct GraphListNode
{
	Graph * graph;
	GraphListNode * next;
};

typedef struct GraphListNode * GraphList;
typedef struct NodeListNode * NodeList;

/*
 * Takes the name of a node and returns the node with that name, or, if that node doesn't
 * exist, adds a node with that name to the global nodelist.
 */
Node * getNode (char * name, NodeList &nodelist);

/*
 * Creates a GraphListNode with an empty graph
 */
GraphListNode * newGraphListNode (GraphListNode * next, Node * start);

void addEdge (Graph * graph, Node * from, Node * to);

/*
 * adds an edge to an annotated graph, at the same time
 * converting it to an annotated edge and counting the number
 * of times it occurs.
 */
void addAnnotatedEdge(AnnotatedGraph * g, Edge * edge);

AnnotatedGraph * summarize (GraphList g, Config * config);

#endif
