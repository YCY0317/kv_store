
#ifndef __KVSTORE_H__
#define __KVSTORE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stddef.h>

#define BUFFER_LENGTH 512

//#define ENABLE_LOG 1
#ifdef ENABLE_LOG

#define LOG(_fmt, ...) fprintf(stdout, "[%s:%d]: " _fmt, __FILE__, __LINE__, ##__VA_ARGS__)

#else

#define LOG(_fmt, ...)

#endif

typedef int (*RCALLBACK)(int fd);

struct conn_item {
    int fd;
    char rbuffer[BUFFER_LENGTH];
    int rlen;
    char wbuffer[BUFFER_LENGTH];
    int wlen;
    union {
        RCALLBACK accept_callback;
        RCALLBACK recv_callback;
    } recv_t;
    RCALLBACK send_callback;
};

extern int epoll_entry(void);
extern int ntyco_entry(void);

extern int kvstore_request(struct conn_item *item);
extern int kvstore_response(void);
extern int init_kvengine(void);
extern int exit_kvengine(void);

extern void* kvstore_malloc(size_t size);
extern void kvstore_free(void* ptr);


#define NETWORK_EPOLL 0
#define NETWORK_NTYCO 1
#define NETWORK_IOURING 2

#define ENABLE_NETWORK_SELECT NETWORK_NTYCO


#define ENABLE_ARRAY_KVENGINE 1
#define ENABLE_RBTREE_KVENGINE 1
#define ENABLE_SKIPTABLE_KVENGINE 1
#define ENABLE_HASH_KVENGINE 1

#if ENABLE_ARRAY_KVENGINE

typedef struct array_s array_t;
extern array_t Array;

extern int kvstore_array_create(array_t *arr);
extern void kvstore_array_destory(array_t *arr);
extern int kvs_array_set(array_t *arr, char *key, char *value);
extern char* kvs_array_get(array_t *arr, char *key);
extern int kvs_array_delete(array_t *arr, char *key);
extern int kvs_array_modify(array_t *arr, char *key, char *value);
extern int kvs_array_count(array_t *arr);
#endif


#if ENABLE_RBTREE_KVENGINE

typedef struct _rbtree retree_t;
extern retree_t Tree;

extern int kvstore_rbtree_create(retree_t *tree);
extern void kvstore_rbtree_destory(retree_t *tree);
extern int kvs_rbtree_set(retree_t *tree, char *key, char *value);
extern char* kvs_rbtree_get(retree_t *tree, char *key);
extern int kvs_rbtree_delete(retree_t *tree, char *key);
extern int kvs_rbtree_modify(retree_t *tree, char *key, char *value);
extern int kvs_rbtree_count(retree_t *tree);
#endif

#endif