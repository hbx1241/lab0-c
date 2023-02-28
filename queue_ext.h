#ifndef LAB0_QUEUE_EXT_H
#define LAB0_QUEUE_EXT_H
#include <stdbool.h>
#include <stddef.h>

#include "harness.h"
#include "list.h"

/**
 * q_linux_lsort() - Sort elements of queue in ascending order using
 * list_sort() in linux kernel
 * @head: header of queue
 */
void q_linux_lsort(struct list_head *head);

/**
 * q_shuffle() - Randomly change elements' order in queue
 * @head: header of queue
 */
void q_shuffle(struct list_head *head);
#endif /* LAB0_QUEUE_EXT_H */
