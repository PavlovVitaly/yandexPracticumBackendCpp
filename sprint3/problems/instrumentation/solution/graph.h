#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include "config.h"
#include "binarytree.h"

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

class HashNode
{
public:
        HashNode (char* n_key, Node* n_node, HashNode* n_next)
        {
                key = n_key;
                node = n_node;
                next = n_next;
        }
	void walk (void (*func)(void *, void *), void *);

        char * key;
        Node * node;
        HashNode * next;
};

class NodeHashTbl
{
public:
        NodeHashTbl (int n_size);
        ~NodeHashTbl ()
        {
                free (table);
        }

        void add (char * key, Node * content);
        Node * get (char * key);
        int size;
        HashNode ** table;
	void walk (void (*func)(void *, void *), void *);
private:
        unsigned int HashString (const char * str);
        NodeHashTbl();
        NodeHashTbl(const NodeHashTbl &);
};

struct Edge
{
	Node * from;
	Node * to;
	Edge * next;

	int key;
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
	BinaryTree * edgetree;
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
Node * getNode (char * name, NodeHashTbl * nodehash);

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
