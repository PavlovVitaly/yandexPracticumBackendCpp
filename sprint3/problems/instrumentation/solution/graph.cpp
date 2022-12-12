#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "graph.h"

void NodeHashTbl::walk (void (*func)(void *, void*), void* arg)
{
	for (int x=0; x<size; x++)
	{
		if (table[x] != NULL)
		{
			table[x]->walk(func, arg);
		}
	}
}

void HashNode::walk (void (*func)(void *, void *), void * arg)
{
	func (node, arg);
	if (next != NULL)
		next->walk(func, arg);
}

unsigned int NodeHashTbl::HashString (const char * str)
{
        unsigned int retval = 0;
        int length = strlen(str);

        unsigned int top5bits = 0xf8000000;
        unsigned int carry = 0;

        const int kleftmove=5;
        const int krightmove=27;

        for (int i=0; i<length; i++)
        {
                carry = retval & top5bits;
                carry = carry >> krightmove;
                retval = retval << kleftmove;
                retval ^= carry;
                retval ^= str[i];
        }
        return retval;
}

NodeHashTbl::NodeHashTbl(int n_size)
{
        size = n_size;
        table = (HashNode **) malloc (sizeof(HashNode*)*size); 
        for (int i=0; i<size; i++)
        {
                table[i] = NULL;
        }
}

void NodeHashTbl::add(char * key, Node * content)
{
        //assert (strcmp (key, content->name) == 0);

        int hkey = HashString (key) % size;
        table[hkey] = new HashNode(key, content, table[hkey]);
}

Node * NodeHashTbl::get(char * key)
{
        HashNode * current_node = table[HashString (key) % size];
        while (current_node != NULL)
        {
                if (strcmp(current_node->key, key) == 0)
                {
                        //assert (strcmp (key, current_node->node->name) == 0);

                        return current_node->node;
                }
                current_node=current_node->next;
        }
        return NULL;
}

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

Node * getNode (char * name, NodeHashTbl * nodehash)
{
        FixName(name);

        Node * retval = nodehash->get(name);

        if (retval == NULL)
        {
                retval = newNode(name);
                nodehash->add(strdup(name), retval);
        }

        return retval;
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

/* 
 * merges the strings in order to get a unique key identifying
 * this pair, using which the binary tree will be sorted.
 *
 * would be better to alternate characters from str1 and str2
 * instead of just concatenating the strings 
 */
int MergeStrings (const char * str1, const char * str2)
{
        int retval = 0;
        while (*str1 != '\0')
                retval += *str1++;
        while (*str2 != '\0')
                retval += *str2++;
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
	retval->key = MergeStrings(from->name, to->name);

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

/* 
 * Looks for the AnnotatedEdge in the tree corresponding
 * to the Edge provided 
 */
AnnotatedEdge * GetEdgeFromTree(BinaryTree * root, Edge * edge)
{
        AnnotatedEdge * temp;
        if (! (temp = (AnnotatedEdge*)(root->get ((void*)&(edge->key)))))
        {
                return NULL;
        }
        while (temp != NULL)
        {
                if (CompareEdges(edge, temp) == 0)
                        return temp;
                temp = temp->next;
        }
        return NULL;
}

/*
 * convert an Edge to an AnnotatedEdge and
 * insert it in the tree
 */     
void InsertEdgeInTree(BinaryTree * tree, Edge * edge)
{
#ifdef DEBUG
        assert (GetEdgeFromTree(tree, edge) == NULL);
#endif  
        void * key = &(edge->key);
        AnnotatedEdge * list;

        if (list = (AnnotatedEdge*)tree->get(key))
        {
                list->next = newAnnotatedEdge(edge, list->next);
        }
        else
        {
                tree->put(key, (void *)newAnnotatedEdge(edge));
        }
}

void addAnnotatedEdge(AnnotatedGraph * g, Edge * edge)
{
	AnnotatedEdge * this_edge = NULL;
	AnnotatedEdge * last_edge = NULL;
	BinaryTree * tree = g->edgetree;

        if (this_edge = GetEdgeFromTree(tree, edge))
        {
                this_edge->n_taken++;
        }
        else
        {
                InsertEdgeInTree(tree, edge);
        }

}

int CompareKey(const void * leftp, const void * rightp)
{
        int left  = *(int*)leftp;
        int right = *(int*)rightp;

        if (left < right)
                return -1;
        else if (left > right)
                return 1;
        else
                return 0;
}

AnnotatedGraph * summarize (GraphList g, Config * config)
{
	AnnotatedGraph * retval = (AnnotatedGraph *) malloc (sizeof(AnnotatedGraph));
	int count = 1;

	retval->edgetree = new BinaryTree(CompareKey);

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
