
#ifndef __KVSTORE_H__
#define __KVSTORE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stddef.h>

#define BUFFER_LENGTH 512

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

extern int kvstore_request(struct conn_item *item);

extern int kvstore_response(void);

extern void* kvstore_malloc(size_t size);
extern void kvstore_free(void* ptr);



#define ENABLE_ARRAY_KENGINE 1
#if ENABLE_ARRAY_KENGINE

struct kvs_array_item {
    char* key;
    char* value;
};
#define KVS_ARRAY_SIZE 1024

extern int kvstore_array_set(char *key, char *value);
extern char* kvstore_array_get(char *key);

#endif

#endif