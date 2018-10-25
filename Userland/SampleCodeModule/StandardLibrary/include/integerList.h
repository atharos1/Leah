#ifndef STD_INTEGER_LIST_H
#define STD_INTEGER_LIST_H

typedef struct Node {
	int data;
	struct Node * next;
} Node;

Node * integerList_init();
Node * insertElement(Node * head, int value);
Node * deleteElement(Node * head, int value);
void printList(Node * head);
int countSpaces(Node * head);
void integerList_destroy(Node * head);

#endif