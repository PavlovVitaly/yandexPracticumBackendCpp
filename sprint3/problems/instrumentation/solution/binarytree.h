#include <stdlib.h>
#include <stdio.h>

#ifndef BINARYTREE_H
#define BINARYTREE_H

class btr_node
{
public:
	~btr_node ();
	btr_node (void * n_key, void * n_content, btr_node * n_left = NULL, btr_node * n_right = NULL)
	{
		key = n_key;
		content = n_content;
		left = n_left;
		right = n_right;
	}
	void * get (void * key, int (*n_compar)(const void*, const void*));
	void put (void * key, void * content, int (*n_compar)(const void*, const void*));
	void * key;
	void * content;
	btr_node * left;
	btr_node * right;
	void walk (void (*func)(void *, void *), void *);
};

class BinaryTree
{
public:
	~BinaryTree ();
	BinaryTree (int (*n_compar)(const void*, const void*))
	{
		compar = n_compar;
		root = NULL;
	}
	void * get (void * key);
	void put (void * key, void * content);
	void walk (void (*func)(void *, void*), void *);

private:
	btr_node * root;
	int (*compar)(const void*, const void*);
};

#endif
