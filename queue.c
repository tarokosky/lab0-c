#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

#include <stdint.h>
#include <sys/time.h>
#include <unistd.h>

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
    if (head == NULL)
        return NULL;
    else {
        head->prev = head;
        head->next = head;
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
    if (head != NULL && head != head->next) {
        element_t *e = list_first_entry(head, element_t, list);
        head->next = head->next->next;
        e->list.next->prev = head;
        if (sp != NULL) {
            strncpy(sp, e->value, bufsize);
            sp[bufsize - 1] = '\0';
        }
        return e;
    }
    return NULL;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (head != NULL && head != head->next) {
        element_t *e = list_last_entry(head, element_t, list);
        head->prev = head->prev->prev;
        e->list.prev->next = head;
        if (sp != NULL) {
            strncpy(sp, e->value, bufsize);
            sp[bufsize - 1] = '\0';
        }
        return e;
    }
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
    if (head == NULL || head->prev == head->next)
        return false;
    else {
        struct list_head *front = head->next;
        struct list_head *rear = head->prev;
        while (front != rear && rear != front->next) {
            front = front->next;
            rear = rear->prev;
        }
        if (front == rear) {
            rear->prev->next = rear->next;
            front->next->prev = front->prev;
            element_t *e = list_entry(rear, element_t, list);
            free(e->value);
            free(e);
        } else if (rear == front->next) {
            rear->next->prev = front;
            front->next = rear->next;
            element_t *e = list_entry(rear, element_t, list);
            free(e->value);
            free(e);
        }
        return true;
    }
}

void _delete_node(struct list_head *node)
{
    struct list_head *front = node->prev;
    struct list_head *rear = node->next;

    front->next = rear;
    rear->prev = front;
    element_t *e = list_entry(node, element_t, list);
    free(e->value);
    free(e);
    return;
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

    if (head == NULL || head->prev == head->next)
        return false;
    else {
        struct list_head *cur = head->next;
        char *target_str = NULL;
        int str_len;
        while (cur != head && cur->next != head) {
            element_t *e1 = list_entry(cur, element_t, list);
            element_t *e2 = list_entry(cur->next, element_t, list);
            if (strcmp(e1->value, e2->value) == 0) {
                str_len = strlen(e1->value) + 1;
                target_str = malloc(sizeof(char) * str_len);
                strncpy(target_str, e1->value, str_len);
                while (cur != head && strcmp(target_str, e1->value) == 0) {
                    cur = cur->next;
                    _delete_node(cur->prev);
                    if (cur == head)
                        break;
                    else
                        e1 = list_entry(cur, element_t, list);
                }
                free(target_str);
            } else {
                cur = cur->next;
            }
        }
        return true;
    }
}

/*
 * Attempt to swap every two adjacent nodes.
 */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/

    if (head == NULL || head->next == head->prev || head->next->next == head)
        return;
    else {
        struct list_head *LL = head;
        struct list_head *L = LL->next;
        struct list_head *R = L->next;
        struct list_head *RR = R->next;
        while (L != head && R != head) {
            LL->next = R;
            L->next = RR;
            L->prev = R;
            R->next = L;
            R->prev = LL;
            RR->prev = L;
            LL = L;
            L = RR;
            R = RR->next;
            RR = R->next;
        }
    }
    return;
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
    struct list_head *cur = head;
    struct list_head *next;
    while (cur != NULL && cur->prev != cur->next) {
        next = cur->next;
        cur->next = cur->prev;
        cur->prev = next;
        if (next == head) {
            next->next = cur;
            break;
        }
        cur = next;
    }
    return;
}

struct list_head *mergeTwoLists(struct list_head *L1, struct list_head *L2)
{
    struct list_head *head = NULL, **ptr = &head, **node;
    for (node = NULL; L1 && L2; *node = (*node)->next) {
        element_t *e1 = list_entry(L1, element_t, list);
        element_t *e2 = list_entry(L2, element_t, list);
        node = (strcasecmp(e1->value, e2->value) < 0) ? &L1 : &L2;
        *ptr = *node;
        ptr = &(*ptr)->next;
    }
    *ptr = (struct list_head *) ((uintptr_t) L1 | (uintptr_t) L2);
    return head;
}

/*
void merge_sort_stack(struct list_head *head)
{
    printf("--  merge_sort_stack  --\n");
    //_show_queue(head);
    struct timeval start_t, end_t;
    unsigned long diff;
    gettimeofday(&start_t, NULL);
    int q_len = q_size(head);
    // struct list_head *result = NULL;
    struct list_head *stack[1000] = {NULL};
    struct list_head *lists[1000000];
    int top = 0;
    int listsSize = 0;
    stack[top] = head->next;
    head->prev->next = NULL;

    while (top >= 0) {
        // printf(" --top: %d, listsSize: %d --\n", top, listsSize);
        struct list_head *left = stack[top--];

        if (left) {
            struct list_head *slow = left;
            struct list_head *fast = left->next;

            while (fast && fast->next) {
                fast = fast->next->next;
                slow = slow->next;
            }
            struct list_head *right = slow->next;
            slow->next = NULL;
            // printf("slow: %p, fast: %p\n", slow, fast);

            stack[++top] = left;
            stack[++top] = right;
        } else {
            lists[listsSize++] = stack[top--];
            // result = mergeTwoLists(result, stack[top--]);
        }
    }
    // merge K sorted lists
    while (listsSize > 1) {
        for (int i = 0, j = listsSize - 1; i < j; i++, j--)
            lists[i] = mergeTwoLists(lists[i], lists[j]);
        listsSize = (listsSize + 1) / 2;
    }

    head->next = lists[0];

    struct list_head *sorted_cur = head->next;
    struct list_head *prev = head;
    while (sorted_cur != NULL) {
        sorted_cur->prev = prev;
        prev = sorted_cur;
        sorted_cur = sorted_cur->next;
    }
    prev->next = head;
    head->prev = prev;
    gettimeofday(&end_t, NULL);
    diff = 1000000 * (end_t.tv_sec - start_t.tv_sec) + end_t.tv_usec -
           start_t.tv_usec;
    printf("Execution time = %lu ms for %d nodes\n\n\n", diff, q_len);

    return;
}

void merge_sort_linkedlist(struct list_head *head)
{
    printf("--  merge_sort_linkedlist  --\n");
    struct timeval start_t, end_t;
    unsigned long diff;
    gettimeofday(&start_t, NULL);
    int q_len = q_size(head);
    struct list_head *start_cur;
    struct list_head *sorted_cur;
    struct list_head *cur0, *cur1, *cur2;
    // struct list_head *end1, *end2;
    struct list_head *tmp1, *tmp2;
    for (int step = 1; step < q_len; step = step << 1) {
        start_cur = head->next;
        sorted_cur = head;
        while (start_cur != head) {
            // Prepare section cursor
            cur0 = start_cur;
            cur1 = start_cur;
            cur2 = start_cur;
            // end1 = end2 = NULL;
            for (int count = 1; count <= step && cur0 != head; count++) {
                if (count == step) {
                    // end1 = cur0;
                    cur2 = cur0->next;
                    cur0->next = NULL;
                    cur0 = cur2;
                    if (cur2 == head)
                        start_cur = head;
                } else if (cur0->next == head) {
                    // end1 = cur0;
                    cur0->next = NULL;
                    cur2 = head;
                    cur0 = head;
                    start_cur = head;
                } else
                    cur0 = cur0->next;
            }

            for (int count = 1; count <= step && cur0 != head; count++) {
                if (count == step) {
                    // end2 = cur0;
                    start_cur = cur0->next;
                    cur0->next = NULL;
                } else if (cur0->next == head) {
                    // end2 = cur0;
                    cur0->next = NULL;
                    start_cur = head;
                    cur0 = head;
                } else
                    cur0 = cur0->next;
            }

            while (cur1 != NULL && cur2 != NULL && cur2 != head) {
                element_t *e1 = list_entry(cur1, element_t, list);
                element_t *e2 = list_entry(cur2, element_t, list);
                int cmp_ret = strcasecmp(e1->value, e2->value);
                if (cmp_ret > 0) {
                    tmp2 = cur2->next;

                    sorted_cur->next = cur2;
                    sorted_cur = cur2;
                    cur2 = tmp2;
                } else if (cmp_ret == 0) {
                    tmp1 = cur1->next;
                    tmp2 = cur2->next;

                    cur1->next = cur2;
                    sorted_cur->next = cur1;
                    sorted_cur = cur2;
                    cur1 = tmp1;
                    cur2 = tmp2;
                } else {
                    tmp1 = cur1->next;

                    sorted_cur->next = cur1;
                    sorted_cur = cur1;
                    cur1 = tmp1;
                }
            }
            if (cur1 != NULL && cur2 == NULL) {
                sorted_cur->next = cur1;
                // if (end1 != NULL)
                //    sorted_cur = end1;
            }
            if (cur1 == NULL && cur2 != NULL) {
                sorted_cur->next = cur2;
                // if (end2 != NULL)
                //    sorted_cur = end2;
            }
            if (cur1 != NULL && cur2 == head) {
                sorted_cur->next = cur1;
                // if (end1 != NULL)
                //    sorted_cur = end1;
            }
            while (sorted_cur->next != NULL)
                sorted_cur = sorted_cur->next;
            sorted_cur->next = head;
        }
    }
    sorted_cur = head->next;
    struct list_head *prev = head;
    while (sorted_cur != head) {
        sorted_cur->prev = prev;
        prev = sorted_cur;
        sorted_cur = sorted_cur->next;
    }
    head->prev = prev;
    gettimeofday(&end_t, NULL);
    diff = 1000000 * (end_t.tv_sec - start_t.tv_sec) + end_t.tv_usec -
           start_t.tv_usec;
    printf("Execution time = %lu ms for %d nodes\n\n\n", diff, q_len);

    return;
}

*/
static struct list_head *mergesort_recursive(struct list_head *head)
{
    if (head == NULL || head->next == NULL)
        return head;
    struct list_head *slow = head;
    struct list_head *fast = head->next;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    struct list_head *mid = slow->next;
    slow->next = NULL;

    struct list_head *left = mergesort_recursive(head);
    struct list_head *right = mergesort_recursive(mid);
    return mergeTwoLists(left, right);
}

void merge_sort_recursive(struct list_head *head)
{
    // struct timeval start_t, end_t;
    // unsigned long diff;
    // gettimeofday(&start_t, NULL);
    // int q_len = q_size(head);
    head->prev->next = NULL;
    head->next = mergesort_recursive(head->next);
    struct list_head *sorted_cur = head->next;
    struct list_head *prev = head;
    while (sorted_cur != NULL) {
        sorted_cur->prev = prev;
        prev = sorted_cur;
        sorted_cur = sorted_cur->next;
    }
    prev->next = head;
    head->prev = prev;

    // gettimeofday(&end_t, NULL);
    // diff = 1000000 * (end_t.tv_sec - start_t.tv_sec) + end_t.tv_usec -
    //       start_t.tv_usec;
    // printf("Execution time = %lu ms for %d nodes\n", diff, q_len);

    return;
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(struct list_head *head)
{
    if (head == NULL || head->prev == head->next || head->next->next == head)
        return;
    else {
        // merge_sort_stack(head);
        // merge_sort_linkedlist(head);
        merge_sort_recursive(head);
    }
    return;
}
