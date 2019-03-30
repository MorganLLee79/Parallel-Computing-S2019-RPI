LOCAL_CFLAGS += -Wall -O3 -Isrc
DEBUG_CFLAGS = $(LOCAL_CFLAGS) -g -Og
LIBS = -pthread -lm

# for debugging
CFLAGS ?= -DROW_LENGTH=1024

EXECUTABLES = assignment4-5 assignment4-5-debug
all: $(EXECUTABLES)

assignment4-5: src/assignment4-5.c clcg4.o
	mpicc $(LOCAL_CFLAGS) $(CFLAGS) $^ $(LIBS) -o $@

assignment4-5-debug: src/assignment4-5.c clcg4.o
	mpicc $(DEBUG_CFLAGS) $(CFLAGS) $^ $(LIBS) -o $@

clcg4.o: src/clcg4.c src/clcg4.h
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXECUTABLES) clcg4.o
