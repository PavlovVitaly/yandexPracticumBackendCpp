#include "binarytree.h"

btr_node::~btr_node ()
{
	if (left != NULL)
		delete (left);
	if (right != NULL)
		delete (right);
}

void * btr_node::get (void * search_key, int (*compar)(const void*, const void*))
{
	if (compar(key, search_key) > 0)
	{
		if (right== NULL)
			return NULL;
		else
			return right->get(search_key, compar);
	}
	else if (compar(key, search_key) == 0)
	{
		return content;
	}
	else
	{
		if (left == NULL)
			return NULL;
		else
			return left->get(search_key, compar);
	}
}

void btr_node::put (void * search_key, void * n_content, int (*compar)(const void*, const void*))
{
	if (compar(key, search_key) > 0)
	{
		if (right== NULL)
			right = new btr_node (search_key, n_content);
		else
			right->put(search_key, n_content, compar);
	}
	else if (compar(key, search_key) == 0)
	{
		fprintf(stderr, "Warning: overwriting duplicate key.");
	}
	else 
	{
		if (left == NULL)
			left = new btr_node (search_key, n_content);
		else
			left->put(search_key, n_content, compar);
	}
}

void * BinaryTree::get (void * key)
{
	if (root == NULL)
		return NULL;
	return root->get(key, compar);
}

void BinaryTree::put (void * key, void * content)
{
	if (root == NULL)
		root = new btr_node(key, content, NULL, NULL);
	else
		root->put(key, content, compar);
}
void BinaryTree::walk (void (*func)(void *, void*), void* arg)
{
	if (root)
		root->walk(func, arg);
}

void btr_node::walk (void (*func)(void *, void*), void* arg)
{
	if (left)
		left->walk(func, arg);
	func (content, arg);
	if (right)
		right->walk(func, arg);
}

BinaryTree::~BinaryTree ()
{
	if (root != NULL)
		delete (root);
}
