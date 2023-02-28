#include "queue_ext.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "linux_list_sort.h"
#include "list.h"
#include "queue.h"


static int cmp(void *pri,
               const struct list_head *l1,
               const struct list_head *l2)
{
    return strcmp(list_entry(l1, element_t, list)->value,
                  list_entry(l2, element_t, list)->value);
}
/* Sorting elements of queue by list_sort from linux source code */
void q_linux_lsort(struct list_head *head)
{
    list_sort(NULL, head, cmp);
}
/* Fisher-Yates shuffle */
void q_shuffle(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *cur = head->prev;
    int cnt = q_size(head);

    srand(time(NULL));
    do {
        int index = rand() % cnt + 1;
        struct list_head *rnd = head, *prev = head;
        while (index--) {
            prev = rnd;
            rnd = rnd->next;
        }
        if (rnd == cur)
            continue;
        // swap two elements
        list_del(rnd);
        list_add(rnd, cur);
        list_del(cur);
        list_add(cur, prev);
        cur = rnd->prev;
    } while (--cnt);
}
