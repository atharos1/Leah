#include "include/integerList.h"
#include "include/stdlib.h"
#include "include/stdio.h"

Node * integerList_init() {
	Node * newNode = NULL;
	return newNode;
}

Node * insertElement(Node * head, int value) {
	Node * newNode = (Node *) malloc(sizeof(Node));
	newNode->data = value;
	newNode->next = NULL;

	if(head == NULL) {
		head = newNode;
		return head;
	}

	Node * curr = head;
	Node * prev = head;

	while(curr != NULL) {
		prev = curr;
		curr = curr->next;
	}
	
	prev->next = newNode;
	return head;
}

Node * deleteElement(Node * head, int value) {
	Node * curr = head;
	Node * prev = head;

	while(curr != NULL && curr->data != value) {
		prev = curr;
		curr = curr->next;
	}

	if(curr != NULL) {
		if(curr == head) {
			head = head->next;
		} else {
			prev->next = curr->next;
		}
		free(curr);
	}
	return head;
}

void printList(Node * head) {
	Node * current = head;
	while(current != NULL) {
		printf("%d ", current->data);
		current = current->next;
	}
	return;
}

int countSpaces(Node * head) {
	int spaces = 0;
	int data;
	Node * curr = head;
	while(curr != NULL) {
		data = curr->data;
		if(data >= 0 && data < 10) {
			spaces += 2;
		} else if(data >= 10 && data < 99) {
			spaces += 3;
		}
		curr = curr->next;
	}
	return spaces;
}

void integerList_destroy(Node * head) {
    Node * n = head, * aux;

    while (n != NULL) {
        aux = n;
        n = n->next;
        free(aux);
    }
}