#include "circularList.h"
#include "memoryManager.h"
#include "drivers/console.h"


#define malloc getMemory //BORRAR ESTA MONSTRUOSIDAD
#define free freeMemory

NODE * createNewNode(void * data) {
	NODE * newNode = (NODE *) malloc (sizeof(NODE));
	newNode -> data = data;
	return newNode;
}

NODE * next(NODE * tail) {
	if (tail == NULL)
		return NULL;

	return tail -> next;
}

void * getFirst(NODE * tail) {
	if (tail == NULL)
		return NULL;

	return tail -> next -> data;
}

void * getLast(NODE * tail) {
	if (tail == NULL)
		return NULL;

	return tail -> data;
}

int length(NODE * tail) {
	NODE * current = tail;
	int i = 1;
	if (tail == NULL) {
		return 0;
	} else {
		current = current -> next;
		while (current != tail) {
			i++;
			current = current -> next;
		}
	}
	return i;
}

int lengthIfLesserThan(NODE * tail, int limit) {
	NODE * current = tail;
	int i = 1;
	if (tail == NULL) {
		return 0;
	} else {
		current = current -> next;
		while (current != tail) {
			i++;
			if(i > limit)
				return -1;

			current = current -> next;
		}
	}
	return i;
}

NODE * insertAtHead(NODE * tail, void * data) {
	NODE * newNode = createNewNode(data);
	if (tail == NULL) {
		tail = newNode;
		newNode -> next = newNode;
	} else {
		newNode -> next = tail -> next;
		tail -> next = newNode;
	}
	return tail;
}

NODE * insertAtEnd(NODE * tail, void * data) {
	// simply insert at head and return the next node pointed by tail
	return insertAtHead(tail, data) -> next;
}

NODE * insertAtArbitrary(NODE * tail, void * data, int location) {
	int len = length(tail), i;
	if (location < 1 || location > len + 1) {
		printf("\nInvalid location to enter data\n");
	} else {
		if (tail == NULL) return insertAtHead(tail, data);
		NODE * newNode = createNewNode(data), * current = tail;
		for (i = 1; i != location; i++) current = current -> next;
		newNode -> next = current -> next;
		current -> next = newNode;
		if (location == len + 1) tail = newNode;
	}
	return tail;
}

NODE * deleteByValue(NODE * tail, void * data, comparator cmp, int * status) {
	NODE * current = tail, * previous;
	*status = 0;
	if (tail == NULL) return tail;
	else if (tail == tail -> next) {
		if ( cmp(tail -> data, data) == 0 ) {
			tail = NULL;
			free(current);
			(*status)++;
		}
		return tail;
	}
	do {
		previous = current;
		current = current -> next;
		if ( cmp(current -> data, data) == 0 ) {
			previous -> next = current -> next;
			if (current == tail) tail = previous;
			free(current);
			(*status)++;
			return tail;
			//current = previous -> next;
			//return tail;
		}
	} while (current != tail);
	return tail;
}

NODE * deleteHead(NODE * tail) {
	return deleteByLocation(tail, 1);
}

NODE * deleteByLocation(NODE * tail, int location) {
	NODE * current = tail, * previous = tail;
	int len = length(tail), i;
	if (location < 1 || location > len) {
		return tail;
	} else if (len == 1) {
		tail = NULL;
		free(current);
	} else {
		current = tail -> next;
		for (i = 1; i < location; i++) {
			previous = current;
			current = current -> next;
		}
		previous -> next = current -> next;
		if (current == tail) tail = previous;
		free(current);
	}

	return tail;
}

NODE * sort(NODE * tail, comparator cmp) {
	if (length(tail) < 2) return tail;
	NODE * ptr1 = tail -> next, * ptr2, * min;
	void * temp;
	// selection sort
	while (ptr1 -> next != tail -> next) {
		min = ptr1;
		ptr2 = ptr1 -> next;
		while (ptr2 != tail -> next) {
			if( cmp(min -> data, ptr2 -> data) > 0 ) min = ptr2;
			ptr2 = ptr2 -> next;
		}
		if (min != ptr1) {
			temp = min -> data;
			min -> data = ptr1 -> data;
			ptr1 -> data = temp;
		}
		ptr1 = ptr1 -> next;
	}
	return tail;
}
