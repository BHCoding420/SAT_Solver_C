#include "list.h"

#include <assert.h>
#include <stdlib.h>

#include "err.h"
#include "util.h"

/**
 * Struct for encapsulating a single list element.
 */
typedef struct ListItem {
    struct ListItem* next;  // pointer to the next element (NULL if last)
    void* data;             // pointer to the data
} ListItem;

List mkList(void) {
    List res;
    res.head = NULL;
    return res;
}

void clearList(List* s) {
    if (s == NULL || s->head == NULL) {
        // List or head is already empty, nothing to clear
        return;
    }

    ListItem* current = s->head;
    ListItem* next;

    while (current != NULL) {
        next = current->next;

        free(current);  // Free the current element
        current = next;
    }

    s->head = NULL;  // Set the head pointer to NULL to indicate an empty list
}

void push(List* s, void* data) {
    if (s == NULL) {
        // Invalid list pointer
        return;
    }

    ListItem* newElement = (ListItem*)malloc(sizeof(ListItem));
    if (newElement == NULL) {
        exit(EXIT_FAILURE);
    }

    newElement->data = data;
    newElement->next = s->head;
    s->head = newElement;
}

void* peek(List* s) {
    if (s->head == NULL) {
        // List is empty
        return NULL;
    }

    return s->head->data;
}

void pop(List* s) {
    if (s == NULL || s->head == NULL) {
        // List is already empty
        return;
    }

    ListItem* temp = s->head;
    s->head = s->head->next;

    free(temp);
}

char isEmpty(List* s) { return (s->head == NULL); }

ListIterator mkIterator(List* list) {
    ListIterator res;
    res.list = list;
    res.prev = NULL;
    res.current = list->head;

    return res;
}

void* getCurr(ListIterator* it) {
    assert(it->current != NULL);
    return it->current->data;
}

void next(ListIterator* it) {
    assert(isValid(it));
    it->prev = it->current;
    it->current = it->current->next;
}

char isValid(ListIterator* it) { return it->current != NULL; }
