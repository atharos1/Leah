#ifndef CIRCULAR_LIST_H
#define CIRCULAR_LIST_H

#define NULL ((void *) 0)

typedef int (*comparator)(void *, void *);

struct Node {
	void * data;
	struct Node * next;
};

typedef struct Node NODE;

NODE * insertAtHead(NODE * tail, void * data);
NODE * insertAtEnd(NODE * tail, void * data);
NODE * deleteByValue(NODE * tail, void * data, comparator cmp, int * cantDeleted, int limit);
NODE * deleteByLocation(NODE * tail, int location);
NODE * deleteHead(NODE * tail);
int length(NODE * tail);
NODE * next(NODE * tail);
void * getFirst(NODE * tail);
void * getLast(NODE * tail);

#endif