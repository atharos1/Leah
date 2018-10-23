#ifndef STD_LINKEDLIST
#define STD_LINKEDLIST

struct node {
    void* data;
    struct node* next;
    struct node* prev;
};

typedef struct node* node_t;

struct linkedList {
    node_t first;
    node_t last;
    node_t current;
    int count;
};

typedef struct linkedList* linkedList_t;

linkedList_t linkedList_new();
int linkedList_count(linkedList_t l);
void linkedList_destroy(linkedList_t l);
void linkedList_offer(linkedList_t l, void* data);
void linkedList_push(linkedList_t l, void* data);
void* linkedList_poll(linkedList_t l);
void* linkedList_pop(linkedList_t l);
int linkedList_isEmpty(linkedList_t l);
void* linkedList_peekFirst(linkedList_t l);
void* linkedList_peekLast(linkedList_t l);

#endif