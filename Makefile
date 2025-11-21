

CC = gcc
FLAGS = -I ./NtyCo/core/ -L ./NtyCo/ -lntyco
SRCS = kv_store.c epoll_entry.c kvstroy_array.c ntyco_entry.c
TESTCASE_SRCS = testcase.c
TARGET = kv_store
SUBDIR = ./NtyCo/
TESTCASE = testcase

OBJS = $(SRCS:.c=.o)

all: $(SUBDIR) $(TARGET) $(TESTCASE)

$(SUBDIR): ECHO
		make -C $@

ECHO:
		@echo $(SUBDIR)

$(TARGET):	$(OBJS)
		$(CC) -o $@ $^ $(FLAGS)

$(TESTCASE): $(TESTCASE_SRCS)
		$(CC) -o $@ $^

%.o: %.c
		$(CC) $(FLAGS) -c $^ -o $@

clean:
		rm -rf $(OBJS) $(TARGET) $(TESTCASE)