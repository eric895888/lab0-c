#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string.h>
#include "queue.h"

void two_list_merge(struct list_head *head_left,
                    struct list_head *head_right,
                    bool descend)
{
    LIST_HEAD(head);

    while (!list_empty(head_left) && !list_empty(head_right)) {
        element_t *right = list_first_entry(head_right, element_t, list);
        element_t *left = list_first_entry(head_left, element_t, list);

        struct list_head *to_move =
            ((strcmp(left->value, right->value) < 0) == !descend)
                ? &left->list
                : &right->list;
        list_del_init(to_move);
        list_add_tail(to_move, &head);
    }
    list_splice_tail_init(head_left, &head);
    list_splice_tail_init(head_right, &head);
    list_splice_tail_init(&head, head_left);  // merge to left
}

void merge_sort(struct list_head *head, int size, bool descend)
{
    if (list_empty(head) || list_is_singular(head)) {
        return;
    }

    struct list_head *node = head;
    for (int i = 0; i < size / 2; i++) {  // find mid
        node = node->next;
    }

    LIST_HEAD(left);
    list_cut_position(&left, head, node);

    merge_sort(&left, size / 2, descend);
    merge_sort(head, size - size / 2, descend);
    two_list_merge(head, &left, descend);
}


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *new_node = malloc(sizeof(struct list_head));

    if (!new_node) {
        return NULL;  // If memory allocation fails, returns NULL.
    }

    INIT_LIST_HEAD(new_node);

    return new_node;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    element_t *curr, *safe;
    list_for_each_entry_safe(curr, safe, head, list)
        q_release_element(curr);
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;

    element_t *new_node = malloc(sizeof(element_t));
    if (!new_node)
        return false;

    new_node->value = strdup(s);
    if (!new_node->value) {
        free(new_node);
        return false;
    }

    // new_node->list.next = head->next;
    // new_node->list.prev = head;
    // head->next->prev = &new_node->list;
    // head->next = &new_node->list;
    list_add(&new_node->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;

    element_t *new_node = malloc(sizeof(element_t));
    if (!new_node)
        return false;

    new_node->value = strdup(s);
    if (!new_node->value) {
        free(new_node);
        return false;
    }
    // new_node->list.next = head;
    // new_node->list.prev = head->prev;
    // head->next->prev = &new_node->list;
    // head->next = &new_node->list;
    list_add_tail(&new_node->list, head);
    return true;
}

/* Remove an element from head of queue */
// sp is a buffer, and the copy operation (such as strncpy) saves the node's
// data into sp, so even if the node is removed, its data can still be used or
// processed in subsequent operations.
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *node = container_of(head->next, element_t, list);
    // head->next = node->list.next;
    // node->list.next->prev = head;
    list_del(&node->list);
    if (sp != NULL && bufsize > 0) {
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';  // be careful
    }

    return node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *node = container_of(head->prev, element_t, list);
    // head->prev = node->list.prev;
    // node->list.prev->next = head;
    list_del(&node->list);
    if (sp != NULL && bufsize > 0) {
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return node;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int count = 0;
    struct list_head *curr = head->next;
    while (curr != head) {
        count++;
        curr = curr->next;
    }
    return count;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return false;
    }

    struct list_head *slow = head->next, *fast = head->next;  // first element

    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }

    list_del(slow);  // delete the node
    q_release_element(
        list_entry(slow, element_t, list));  // free  memory of the node
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return false;
    }

    bool dup = false;
    element_t *curr, *safe;
    list_for_each_entry_safe(curr, safe, head, list) {
        if (&safe->list != head && !strcmp(curr->value, safe->value)) {
            list_del(&curr->list);
            q_release_element(curr);
            dup = true;
        } else if (dup) {
            list_del(&curr->list);
            q_release_element(curr);
            dup = false;
        }
    }

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // if (head && !list_empty(head) && !list_is_singular(head)) {
    //     struct list_head *curr = head->next;
    //     while (curr != head && curr->next != head) {
    //         struct list_head *tmp = curr->next;
    //         list_del(curr);
    //         list_add(curr, tmp);
    //         curr = curr->next;
    //     }
    // }
    q_reverseK(head, 2);
}

/* Reverse elements in queue */
// head <-> A <-> B <-> C <-> D <-> head ,curr=A
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return;
    }

    struct list_head *prev = head->prev, *curr = head, *next = head->next;

    while (next != head) {
        next = curr->next;
        curr->next = prev;
        curr->prev = next;
        prev = curr;
        curr = next;
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head))
        return;

    struct list_head *curr, *safe, *tmp_head = head;
    LIST_HEAD(splited_list_head);  // Temporary list of K nodes
    int len = 0;
    list_for_each_safe(curr, safe, head) {
        len++;
        if (len == k) {
            list_cut_position(&splited_list_head, tmp_head, curr);
            q_reverse(&splited_list_head);
            list_splice_init(&splited_list_head, tmp_head);
            len = 0;
            tmp_head = safe->prev;
        }
    }
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return;
    merge_sort(head, q_size(head), descend);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    int num = 0;
    struct list_head *node = head->prev, *node_prev = node->prev;

    element_t *que = list_entry(node, element_t, list);
    element_t *que_prev = list_entry(node_prev, element_t, list);

    while (&que_prev->list != head) {
        if (strcmp(que->value, que_prev->value) > 0) {
            que_prev = list_entry(que_prev->list.prev, element_t, list);
            que = list_entry(que->list.prev, element_t, list);
            ++num;
        } else {
            list_del(&que_prev->list);
            q_release_element(que_prev);
            que_prev = list_entry(que->list.prev, element_t, list);
        }
    }
    return ++num;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    int num = 0;
    struct list_head *node = head->prev, *node_prev = node->prev;

    element_t *que = list_entry(node, element_t, list);
    element_t *que_prev = list_entry(node_prev, element_t, list);

    while (&que_prev->list != head) {
        if (strcmp(que->value, que_prev->value) < 0) {
            que_prev = list_entry(que_prev->list.prev, element_t, list);
            que = list_entry(que->list.prev, element_t, list);
            ++num;
        } else {
            list_del(&que_prev->list);
            q_release_element(que_prev);
            que_prev = list_entry(que->list.prev, element_t, list);
        }
    }
    return ++num;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return 0;

    LIST_HEAD(tmp);
    queue_contex_t *first = list_first_entry(head, queue_contex_t, chain);
    queue_contex_t *curr;

    list_for_each_entry(curr, head, chain) {
        list_splice_init(curr->q, &tmp);
    }

    q_sort(&tmp, descend);
    list_splice_init(&tmp, first->q);

    return q_size(first->q);
}
