

CC = gcc
FLAGS = -I ./NtyCo/core/ -L ./NtyCo/ -lntyco
SRCS = kv_store.c epoll_entry.c kvstroy_array.c ntyco_entry.c
TARGET = kv_store
SUBDIR = ./NtyCo/

OBJS = $(SRCS:.c=.o)

all: $(SUBDIR) $(TARGET)

$(SUBDIR): ECHO
		make -C $@

ECHO:
		@echo $(SUBDIR)

$(TARGET):	$(OBJS)
		$(CC) -o $@ $^ $(FLAGS)

%.o: %.c
		$(CC) $(FLAGS) -c $^ -o $@

clean:
		rm -rf $(OBJS) $(TARGET)