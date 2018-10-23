#include "include/linkedList.h"

#define TRUE 1
#define FALSE 0

#define NULL ((void*)0)

linkedList_t linkedList_new() {
    linkedList_t ret = malloc(sizeof(struct linkedList));
    ret->count = 0;
    ret->first = ret->last = ret->current = NULL;
    return ret;
}

int linkedList_count(linkedList_t l) { return l->count; }

void linkedList_destroy(linkedList_t l) {
    node_t n = l->first, aux;

    while (n != NULL) {
        aux = n;
        n = n->next;
        free(aux);
    }

    free(l);
}

void linkedList_offer(linkedList_t l, void* data) {
    node_t n = malloc(sizeof(struct node));
    n->data = data;
    n->next = NULL;
    n->prev = l->last;

    if (l->last != NULL) l->last->next = n;

    l->last = n;
    if (l->first == NULL) l->first = n;

    l->count++;
}

void linkedList_push(linkedList_t l, void* data) {
    node_t n = malloc(sizeof(struct node));

    n->data = data;
    n->next = l->first;
    n->prev = NULL;

    if (l->first != NULL) l->first->prev = n;

    l->first = n;
    if (l->last == NULL) l->last = n;

    l->count++;
}

void* linkedList_poll(linkedList_t l) {
    if (linkedList_isEmpty(l)) return NULL;

    node_t aux = l->first;
    void* ret = aux->data;

    if (l->first->next != NULL) l->first->next->prev = NULL;

    l->first = l->first->next;

    if (l->last == aux) l->last = NULL;

    free(aux);
    l->count--;

    return ret;
}

int linkedList_removeByValue(linkedList_t l, void * value, int limit) {
    if (linkedList_isEmpty(l)) return NULL;

    node_t aux = l->first, auxDelete;
    int deleted = 0;

    while(aux != NULL && deleted < limit) {
        if(aux->data == value) {
            if(aux != l->first)
                aux->prev->next = aux->next;
            else
                l->first = aux->next;

            if(aux != l->last)
                aux->next->prev = aux->prev;
            else
                l->last = aux->prev;

            auxDelete = aux;
            aux = aux->next;
            free(auxDelete);
            l->count--;
            deleted++;
        } else {
            aux = aux->next;
        }
    }

    return deleted;
}

void* linkedList_pop(linkedList_t l) {
    if (linkedList_isEmpty(l)) return NULL;

    node_t aux = l->last;
    void* ret = aux->data;

    if (l->last->prev != NULL) l->last->prev->next = NULL;

    l->last = l->last->prev;

    if (l->first == aux) l->first = NULL;

    free(aux);
    l->count--;

    return ret;
}

int linkedList_isEmpty(linkedList_t l) { return (l->first == NULL); }

void* linkedList_peekFirst(linkedList_t l) {
    if (linkedList_isEmpty(l)) return NULL;

    return l->first->data;
}

void* linkedList_peekLast(linkedList_t l) {
    if (linkedList_isEmpty(l)) return NULL;

    return l->last->data;
}