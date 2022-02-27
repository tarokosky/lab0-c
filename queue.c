#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
struct list_head *q_new()
{
    struct list_head *head =
        (struct list_head *) malloc(sizeof(struct list_head));
    // struct element_t *node = (struct element_t *)malloc (sizeof(element_t));
    if (head == NULL)
        return NULL;
    else {
        head->prev = head;
        head->next = head;
        printf(".head => %p\n", head);
        return head;
    }
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    struct list_head *cur;
    if (l != NULL) {
        cur = l->next;
        while (cur != l) {
            element_t *e = list_entry(cur, element_t, list);
            cur = cur->next;
            free(e->value);
            free(e);
        }
        free(l);
    }
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(struct list_head *head, char *s)
{
    int str_len = strlen(s) + 1;
    element_t *node = malloc(sizeof(element_t));
    char *str = malloc(sizeof(char) * str_len);

    // printf(" .. %p, %s\n", node, s);

    if (node == NULL || head == NULL || str == NULL) {
        if (node)
            free(node);
        if (str)
            free(str);
        return false;
    } else {
        strncpy(str, s, str_len);
        node->value = str;
        node->list.prev = head;
        node->list.next = head->next;
        head->next->prev = &node->list;
        head->next = &node->list;
        return true;
    }
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(struct list_head *head, char *s)
{
    int str_len = strlen(s) + 1;
    element_t *node = malloc(sizeof(element_t));
    char *str = malloc(sizeof(char) * str_len);

    // printf(" .. %p, %s\n", node, s);

    if (node == NULL || head == NULL || str == NULL) {
        if (node)
            free(node);
        if (str)
            free(str);
        return false;
    } else {
        strncpy(str, s, str_len);
        node->value = str;
        node->list.prev = head->prev;
        node->list.next = head;
        head->prev->next = &node->list;
        head->prev = &node->list;
        return true;
    }
}

/*
 * Attempt to remove element from head of queue.
 * Return target element.
 * Return NULL if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 *
 * NOTE: "remove" is different from "delete"
 * The space used by the list element and the string should not be freed.
 * The only thing "remove" need to do is unlink it.
 *
 * REF:
 * https://english.stackexchange.com/questions/52508/difference-between-delete-and-remove
 */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    return NULL;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    return NULL;
}

/*
 * WARN: This is for external usage, don't modify it
 * Attempt to release element.
 */
void q_release_element(element_t *e)
{
    free(e->value);
    free(e);
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/*
 * Delete the middle node in list.
 * The middle node of a linked list of size n is the
 * ⌊n / 2⌋th node from the start using 0-based indexing.
 * If there're six element, the third member should be return.
 * Return true if successful.
 * Return false if list is NULL or empty.
 */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    return true;
}

/*
 * Delete all nodes that have duplicate string,
 * leaving only distinct strings from the original list.
 * Return true if successful.
 * Return false if list is NULL.
 *
 * Note: this function always be called after sorting, in other words,
 * list is guaranteed to be sorted in ascending order.
 */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/*
 * Attempt to swap every two adjacent nodes.
 */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(struct list_head *head)
{
    /*
    struct list_head *cur = head;
    struct list_head *prev, *next;
    int count = 1;
    printf("head: %p, %p, %p\n\n", cur->prev, cur, cur->next);
    cur = head->next;
    while (cur != head) {
        element_t *e = list_entry(cur, element_t, list);
        printf("%d => c: %p, %p, %p, %s\n", count, cur->prev, cur, cur->next,
               e->value);
        count++;
        cur = cur->next;
    }

    printf("\n\n");


    count = 1;
    cur = head->prev;
    while (cur != NULL && cur != head) {
        if (count > 10) {
            break;
        }
        if (cur != head) {
            element_t *e = list_entry(cur, element_t, list);
            printf("%d => %s\n", count, e->value);
        }
        prev = cur->prev;
        next = cur->next;
        printf("%d => head: %p, cursor => %p, prev => %p, next => %p\n", count,
    head, cur, prev, next); printf("%d => 1. c: %p, %p, %p\n", count, cur->prev,
    cur, cur->next); printf("%d => 1. p: %p, %p, %p\n", count, prev->prev, prev,
    prev->next); printf("%d => 1. n: %p, %p, %p\n", count, next->prev, next,
    next->next); cur->prev = next; cur->next = prev; printf("%d => 2. c: %p, %p,
    %p\n", count, cur->prev, cur, cur->next); printf("%d => 2. p: %p, %p, %p\n",
    count, prev->prev, prev, prev->next); printf("%d => 2. n: %p, %p, %p\n",
    count, next->prev, next, next->next); cur = prev; printf("%d => 3. c: %p,
    %p, %p\n", count, cur->prev, cur, cur->next); printf("%d => 3. p: %p, %p,
    %p\n", count, prev->prev, prev, prev->next); printf("%d => 3. n: %p, %p,
    %p\n", count, next->prev, next, next->next); count++;
    }

    printf("------------------------------------------\n");

    cur = head->next;
    while (cur != head) {
        element_t *e = list_entry(cur, element_t, list);
        printf("%d => c: %p, %p, %p, %s\n", count, cur->prev, cur, cur->next,
               e->value);
        count++;
        cur = cur->next;
    }

    */
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(struct list_head *head) {}
