
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

extern void* kvstore_malloc(size_t size);
extern void kvstore_free(void* ptr);


#define NETWORK_EPOLL 0
#define NETWORK_NTYCO 1
#define NETWORK_IOURING 2

#define ENABLE_NETWORK_SELECT NETWORK_NTYCO

#define ENABLE_ARRAY_KENGINE 1
#if ENABLE_ARRAY_KENGINE

struct kvs_array_item {
    char* key;
    char* value;
};
#define KVS_ARRAY_SIZE 1024

extern int kvstore_array_set(char *key, char *value);
extern char* kvstore_array_get(char *key);
extern int kvstore_array_delete(char *key);
extern int kvstore_array_modify(char *key, char *value);

#endif

#endif