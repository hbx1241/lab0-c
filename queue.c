#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *p = malloc(sizeof(struct list_head));
    if (!p)
        return NULL;
    else {
        INIT_LIST_HEAD(p);
        return p;
    }
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    // Keep deleting first entry of the queue until list is empty
    while (l && !list_empty(l)) {
        element_t *ptr = list_first_entry(l, element_t, list);
        list_del(&ptr->list);
        q_release_element(ptr);
    }
    // free head
    free(l);
}
bool q_insert(struct list_head *head, char *s, bool h)
{
    if (!head || !s)
        return false;
    element_t *eptr = malloc(sizeof(element_t));
    if (!eptr)
        return false;
    int string_size = strlen(s);
    eptr->value = malloc(sizeof(char) * (string_size + 1));
    if (!eptr->value) {
        q_release_element(eptr);
        return false;
    }
    memcpy(eptr->value, s, string_size + 1);
    INIT_LIST_HEAD(&eptr->list);
    if (h)
        list_add(&eptr->list, head);
    else
        list_add_tail(&eptr->list, head);
    return true;
}
/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    return q_insert(head, s, true);
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    return q_insert(head, s, false);
}

element_t *q_remove(struct list_head *head, char *sp, size_t bufsize, bool h)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *ptr = h ? list_entry(head->next, element_t, list)
                       : list_entry(head->prev, element_t, list);
    int len = strlen(ptr->value);
    int num = len > bufsize - 1 ? bufsize - 1 : len;
    if (sp && num > 0) {
        strncpy(sp, ptr->value, num);
        sp[num] = '\0';
    }
    list_del(&ptr->list);
    return ptr;
}
/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    return q_remove(head, sp, bufsize, true);
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    return q_remove(head, sp, bufsize, false);
}

/* Return number of elements in queue */
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

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;
    struct list_head *slow, *fast;
    for (slow = head->next, fast = head->next;
         fast != head && fast->next != head; fast = fast->next->next)
        slow = slow->next;
    if (fast == head)
        slow = slow->prev;
    list_del(slow);
    q_release_element(list_entry(slow, element_t, list));
    return true;
}

void q_sort(struct list_head *head);
/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head) || list_is_singular(head))
        return false;
    q_sort(head);
    struct list_head *ptr = head->next;
    while (ptr != head) {
        element_t *ecur = (element_t *) list_entry(ptr, element_t, list);
        bool dup = false;
        while (ptr->next != head) {
            element_t *enext =
                (element_t *) list_entry(ptr->next, element_t, list);
            if (!strcmp(ecur->value, enext->value)) {
                dup = true;
                list_del(&enext->list);
                q_release_element(enext);
            } else
                break;
        }
        ptr = ptr->next;
        if (dup) {
            struct list_head *del = ptr->prev;
            list_del(del);
            q_release_element(list_entry(del, element_t, list));
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *cur;
    cur = head->next;
    while (cur != head && cur->next != head) {
        struct list_head *tmp;
        tmp = cur->next;
        list_del(tmp);
        list_move_tail(tmp, cur);
        cur = cur->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *ptr = head->next;

    while (ptr->next != head) {
        list_move(ptr->next, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *ptr = head->next;
    while (ptr != head) {
        struct list_head *tmp = ptr;
        int cnt = 1;
        while (tmp != head && cnt < k) {
            tmp = tmp->next;
            cnt++;
        }
        if (tmp == head)
            break;
        // Reverse next k elements in the queue from tmp
        tmp = ptr->prev;
        for (int i = 0; i < k - 1; i++) {
            list_move(ptr->next, tmp);
        }
        ptr = ptr->next;
    }
}

struct list_head *merge_two_list(struct list_head *l, struct list_head *r)
{
    struct list_head *head = NULL;
    struct list_head **ptr = &head;

    while (l && r) {
        element_t *le = list_entry(l, element_t, list);
        element_t *re = list_entry(r, element_t, list);
        if (strcmp(le->value, re->value) <= 0) {
            *ptr = l;
            l = l->next;
        } else {
            *ptr = r;
            r = r->next;
        }
        ptr = &(*ptr)->next;
    }
    *ptr = !l ? r : l;
    return head;
}
struct list_head *merge_sort(struct list_head *list)
{
    if (!list || !list->next)
        return list;

    struct list_head *slow = list, *fast = list, *prev = NULL;
    for (; fast && fast->next; fast = fast->next->next) {
        prev = slow;
        slow = slow->next;
    }
    prev->next = NULL;
    list = merge_sort(list);
    slow = merge_sort(slow);
    return merge_two_list(list, slow);
}
/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *ptr = head->next;
    head->prev->next = NULL;
    INIT_LIST_HEAD(head);
    ptr = merge_sort(ptr);
    struct list_head **cur = &ptr;
    while ((*cur)->next) {
        (*cur)->next->prev = *cur;
        cur = &(*cur)->next;
    }
    (*cur)->next = ptr;
    ptr->prev = *cur;
    list_add_tail(head, ptr);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    else if (list_is_singular(head))
        return 1;

    struct list_head *ptr = head->prev;
    while (ptr != head) {
        for (struct list_head *i = head->next; i != ptr;) {
            element_t *ie = list_entry(i, element_t, list);
            element_t *pe = list_entry(ptr, element_t, list);
            i = i->next;
            if (strcmp(ie->value, pe->value) < 0) {
                list_del(&ie->list);
                q_release_element(ie);
            }
        }
        ptr = ptr->prev;
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head) || list_is_singular(head))
        return 0;

    bool find = true;
    queue_contex_t *f_contex = list_entry(head->next, queue_contex_t, chain);
    while (find) {
        struct list_head *min = NULL;
        char *str_min = NULL;
        struct list_head *cur = head->next->next;
        find = false;
        while (cur != head) {
            queue_contex_t *cur_q = list_entry(cur, queue_contex_t, chain);
            if (cur_q->q && !list_empty(cur_q->q)) {
                // first element of queue
                element_t *cur_e = list_entry(cur_q->q->next, element_t, list);
                if (!str_min || (strcmp(str_min, cur_e->value) > 0)) {
                    min = &cur_e->list;
                    str_min = cur_e->value;
                    find = true;
                }
            }
            cur = cur->next;
        }
        if (find) {
            list_move_tail(min, f_contex->q);
            q_sort(f_contex->q);
        }
    }
    return 2;
}
