

#include "kv_store.h"

#define KVSTORE_MAX_TOKENS 128

const char *commands[] = {"SET", "GET", "DEL", "MOD"};
enum {
    KVS_CMD_START = 0,
    KVS_CMD_SET = KVS_CMD_START ,
    KVS_CMD_GET,
    KVS_CMD_DEL,
    KVS_CMD_MOD,
    KVS_CMD_COUNT
};

void* kvstore_malloc(size_t size) {
    return malloc(size);
}
void kvstore_free(void* ptr) {
    return free(ptr);
}


int kvstore_split_token(char *msg, char **tokens) {
    if(msg == NULL || tokens == NULL)
        return -1;
    
    int idx = 0;
    char *token = strtok(msg, " ");
    while(token != NULL) {
        tokens[idx++] = token;
        token = strtok(NULL, " ");
    }
    return idx;
}

int kvstore_parser_protocol(struct conn_item *item, char **tokens, int count) {
    if(!item || tokens[0] == NULL || count == 0)
        return -1;
    
    int cmd=KVS_CMD_START;
    for(cmd=KVS_CMD_START; cmd<KVS_CMD_COUNT; cmd++) {
        if(strcmp(commands[cmd], tokens[0]) == 0)
            break;
    }
    char* msg = item->wbuffer;
    char* key = tokens[1];
    char* value = tokens[2];
    memset(msg, 0, BUFFER_LENGTH);
    switch(cmd) {
        case KVS_CMD_SET: {
            int res = kvstore_array_set(key, value);
            if(!res) {
                snprintf(msg, BUFFER_LENGTH, "SUCCESS");
            }
            else {
                snprintf(msg, BUFFER_LENGTH, "FAILED");
            }
            break;
        }
        case KVS_CMD_GET: {
            char* val = kvstore_array_get(key);
            if(val) {
                snprintf(msg, BUFFER_LENGTH, "%s", val);
            }
            else {
                snprintf(msg, BUFFER_LENGTH, "NO EXIST");
            }
            break;
        }
        case KVS_CMD_DEL: {
            int res = kvstore_array_delete(key);
			if (res < 0) {
				snprintf(msg, BUFFER_LENGTH, "%s", "ERROR");
			}
            else if (res == 0) {
				snprintf(msg, BUFFER_LENGTH, "%s", "SUCCESS");
			}
            else {
				snprintf(msg, BUFFER_LENGTH, "NO EXIST");
			}
            break;
        }
        case KVS_CMD_MOD: {
            int res = kvstore_array_modify(key, value);
			if (res < 0) {
				snprintf(msg, BUFFER_LENGTH, "%s", "ERROR");
			}
            else if (res == 0) {
				snprintf(msg, BUFFER_LENGTH, "%s", "SUCCESS");
			}
            else {
				snprintf(msg, BUFFER_LENGTH, "NO EXIST");
			}
            break;
        }
        default: {
            printf("cmd: %s\n", commands[cmd]);
            assert(0);
        }
    }
}

int kvstore_request(struct conn_item *item) {
    LOG("recv :%s\n", item->rbuffer);

    char *msg = item->rbuffer;
    char *tokens[KVSTORE_MAX_TOKENS];

    int count = kvstore_split_token(msg, tokens);
    for(int idx=0; idx<count; idx++) {
        LOG("idx: %s\n", tokens[idx]);
    }

    kvstore_parser_protocol(item, tokens, count);
    return 0;
}

int kvstore_response(void) {


}


int main() {
#if(ENABLE_NETWORK_SELECT == NETWORK_EPOLL)
    epoll_entry();
#elif(ENABLE_NETWORK_SELECT == NETWORK_NTYCO)
    ntyco_entry();
#elif(ENABLE_NETWORK_SELECT == NETWORK_IOURING)

#endif
    return 0;
}


