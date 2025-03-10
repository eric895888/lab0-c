#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"



/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *new_node = malloc(sizeof(struct list_head));

    if (!new_node) {
        return NULL;  // If memory allocation fails, returns NULL.
    }

    new_node->prev = new_node;
    new_node->next = new_node;

    return new_node;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return;
    }

    struct list_head *curr = head->next;
    struct list_head *tmp = NULL;

    while (curr != head) {
        tmp = curr->next;
        free(curr);
        curr = tmp;
    }

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

    new_node->list.next = head->next;
    new_node->list.prev = head;
    head->next->prev = &new_node->list;
    head->next = &new_node->list;

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
    new_node->list.next = head;
    new_node->list.prev = head->prev;
    head->next->prev = &new_node->list;
    head->next = &new_node->list;

    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *node = head->next;
    head->next = node->list.next;
    node->list.next->prev = head;

    if (sp && node->value) {
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *node = (element_t *) head->prev;
    head->prev = node->list.prev;
    node->list.prev->next = head;

    if (sp && node->value) {
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

    struct list_head *slow = head, *fast = head;

    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }

    struct list_head *mid_node = slow;

    list_del(mid_node);

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return false;
    }

    struct list_head *pos, *next;
    int count = 0;

    list_for_each_safe(pos, next, head) {
        struct list_head *current_node = pos;
        struct list_head *next_node = next;

        if (next_node == head) {
            break;
        }

        if (current_node->next != head && current_node->next == next_node) {
            list_del(next_node);
            count++;
        }
    }
    if (count > 0)
        return true;

    return 0;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (head && !list_empty(head) && !list_is_singular(head)) {
        struct list_head *curr = head->next;
        while (curr != head && curr->next != head) {
            struct list_head *tmp = curr->next;
            list_del(curr);
            list_add(curr, tmp);
            curr = curr->next;
        }
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return;
    }

    struct list_head *pos = head->next;
    struct list_head *next_node = NULL;

    while (pos != head) {
        next_node = pos->next;
        pos->next = pos->prev;
        pos->prev = next_node;
        pos = next_node;
    }

    head->next->prev = head;
    head->prev = pos->prev;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return;

    merge_sort(head);
    if (descend)
        q_reverse(head);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    struct list_head *i, *j;
    element_t *elem_j;

    for (i = head->next; i != head; i = i->next) {
        element_t *elem_i;
        elem_i = list_entry(i, element_t, list);
        for (j = i->next; j != head; j = j->next) {
            elem_j = list_entry(j, element_t, list);

            if (strcmp(elem_i->value, elem_j->value) > 0) {
                list_del(i);
                list_del(j);
                list_add(i, j);
            }
        }
    }
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    struct list_head *i, *j;
    element_t *elem_j;

    for (i = head->next; i != head; i = i->next) {
        element_t *elem_i;
        elem_i = list_entry(i, element_t, list);  // 取得元素
        for (j = i->next; j != head; j = j->next) {
            elem_j = list_entry(j, element_t, list);
            // 比較並進行升序排序
            if (strcmp(elem_i->value, elem_j->value) < 0) {
                // 交換元素的位置
                list_del(i);
                list_del(j);
                list_add(i, j);
            }
        }
    }
    return 1;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
struct list_head *merge_sort(struct list_head *new_head)
{
    if (new_head == new_head->next)
        return new_head;

    struct list_head *left = new_head;
    struct list_head *right = find_mid(new_head);

    if (list_is_singular(left)) {
        list_del_init(left);
        list_del_init(right);
    } else {
        right->prev->next = left;
        left->prev->next = right;
        struct list_head *tp = left->prev;
        left->prev = right->prev;
        right->prev = tp;
    }

    left = merge_sort(left);
    right = merge_sort(right);
    left->prev->next = NULL;
    right->prev->next = NULL;

    for (struct list_head *tmp = NULL; left || right;) {
        if (!right ||
            (left &&
             ((strcmp(list_entry(left, element_t, list)->value,
                      list_entry(right, element_t, list)->value)) < 0))) {
            if (!tmp) {
                tmp = new_head = left;
                left = left->next;
                if (left != NULL) {
                    left->prev = tmp->prev;
                }
                INIT_LIST_HEAD(tmp);
            } else {
                tmp = left;
                left = left->next;
                if (left != NULL) {
                    left->prev = tmp->prev;
                }
                list_add_tail(tmp, new_head);
            }
        } else {
            if (!tmp) {
                tmp = new_head = right;
                right = right->next;
                if (right != NULL) {
                    right->prev = tmp->prev;
                }
                INIT_LIST_HEAD(tmp);
            } else {
                tmp = right;
                right = right->next;
                if (right != NULL) {
                    right->prev = tmp->prev;
                }
                list_add_tail(tmp, new_head);
            }
        }
    }
    return new_head;
}
struct list_head *find_mid(struct list_head *head)
{
    struct list_head *forw = head, *back = head->prev;
    while (forw != back && forw->next != back) {
        forw = forw->next;
        back = back->prev;
    }
    return back;
}