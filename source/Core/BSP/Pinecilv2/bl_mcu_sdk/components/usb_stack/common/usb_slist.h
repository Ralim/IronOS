#ifndef __USB_SLIST_H__
#define __USB_SLIST_H__

#include "string.h"
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * usb_container_of - return the member address of ptr, if the type of ptr is the
 * struct type.
 */
#define usb_container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))

/**
 * Single List structure
 */
struct usb_slist_node {
    struct usb_slist_node *next; /**< point to next node. */
};
typedef struct usb_slist_node usb_slist_t; /**< Type for single list. */

/**
 * @brief initialize a single list
 *
 * @param l the single list to be initialized
 */
static inline void usb_slist_init(usb_slist_t *l)
{
    l->next = NULL;
}

static inline void usb_slist_add_head(usb_slist_t *l, usb_slist_t *n)
{
    n->next = l->next;
    l->next = n;
}

static inline void usb_slist_add_tail(usb_slist_t *l, usb_slist_t *n)
{
    while (l->next) {
        l = l->next;
    }

    /* append the node to the tail */
    l->next = n;
    n->next = NULL;
}

static inline void usb_slist_insert(usb_slist_t *l, usb_slist_t *next, usb_slist_t *n)
{
    if (!next) {
        usb_slist_add_tail(next, l);
        return;
    }

    while (l->next) {
        if (l->next == next) {
            l->next = n;
            n->next = next;
        }

        l = l->next;
    }
}

static inline usb_slist_t *usb_slist_remove(usb_slist_t *l, usb_slist_t *n)
{
    /* remove slist head */
    while (l->next && l->next != n) {
        l = l->next;
    }

    /* remove node */
    if (l->next != (usb_slist_t *)0) {
        l->next = l->next->next;
    }

    return l;
}

static inline unsigned int usb_slist_len(const usb_slist_t *l)
{
    unsigned int len = 0;
    const usb_slist_t *list = l->next;

    while (list != NULL) {
        list = list->next;
        len++;
    }

    return len;
}

static inline unsigned int usb_slist_contains(usb_slist_t *l, usb_slist_t *n)
{
    while (l->next) {
        if (l->next == n) {
            return 0;
        }

        l = l->next;
    }

    return 1;
}

static inline usb_slist_t *usb_slist_head(usb_slist_t *l)
{
    return l->next;
}

static inline usb_slist_t *usb_slist_tail(usb_slist_t *l)
{
    while (l->next) {
        l = l->next;
    }

    return l;
}

static inline usb_slist_t *usb_slist_next(usb_slist_t *n)
{
    return n->next;
}

static inline int usb_slist_isempty(usb_slist_t *l)
{
    return l->next == NULL;
}

/**
 * @brief initialize a slist object
 */
#define USB_SLIST_OBJECT_INIT(object) \
    {                                 \
        NULL                          \
    }

/**
 * @brief initialize a slist object
 */
#define USB_SLIST_DEFINE(slist) \
    usb_slist_t slist = { NULL }

/**
 * @brief get the struct for this single list node
 * @param node the entry point
 * @param type the type of structure
 * @param member the name of list in structure
 */
#define usb_slist_entry(node, type, member) \
    usb_container_of(node, type, member)

/**
 * usb_slist_first_entry - get the first element from a slist
 * @ptr:    the slist head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the slist_struct within the struct.
 *
 * Note, that slist is expected to be not empty.
 */
#define usb_slist_first_entry(ptr, type, member) \
    usb_slist_entry((ptr)->next, type, member)

/**
 * usb_slist_tail_entry - get the tail element from a slist
 * @ptr:    the slist head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the slist_struct within the struct.
 *
 * Note, that slist is expected to be not empty.
 */
#define usb_slist_tail_entry(ptr, type, member) \
    usb_slist_entry(usb_slist_tail(ptr), type, member)

/**
 * usb_slist_first_entry_or_null - get the first element from a slist
 * @ptr:    the slist head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the slist_struct within the struct.
 *
 * Note, that slist is expected to be not empty.
 */
#define usb_slist_first_entry_or_null(ptr, type, member) \
    (usb_slist_isempty(ptr) ? NULL : usb_slist_first_entry(ptr, type, member))

/**
 * usb_slist_for_each - iterate over a single list
 * @pos:    the usb_slist_t * to use as a loop cursor.
 * @head:   the head for your single list.
 */
#define usb_slist_for_each(pos, head) \
    for (pos = (head)->next; pos != NULL; pos = pos->next)

#define usb_slist_for_each_safe(pos, next, head)    \
    for (pos = (head)->next, next = pos->next; pos; \
         pos = next, next = pos->next)

/**
 * usb_slist_for_each_entry  -   iterate over single list of given type
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your single list.
 * @member: the name of the list_struct within the struct.
 */
#define usb_slist_for_each_entry(pos, head, member)                 \
    for (pos = usb_slist_entry((head)->next, typeof(*pos), member); \
         &pos->member != (NULL);                                    \
         pos = usb_slist_entry(pos->member.next, typeof(*pos), member))

#define usb_slist_for_each_entry_safe(pos, n, head, member)          \
    for (pos = usb_slist_entry((head)->next, typeof(*pos), member),  \
        n = usb_slist_entry(pos->member.next, typeof(*pos), member); \
         &pos->member != (NULL);                                     \
         pos = n, n = usb_slist_entry(pos->member.next, typeof(*pos), member))

#ifdef __cplusplus
}
#endif

#endif