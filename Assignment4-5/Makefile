LOCAL_CFLAGS += -Wall -O5 -Isrc
DEBUG_CFLAGS = $(LOCAL_CFLAGS) -g -Og -DDEBUG_MODE
LIBS = -pthread

MPICC ?= mpicc

EXECUTABLES = assignment4-5 assignment4-5-debug
all: $(EXECUTABLES)

assignment4-5: src/assignment4-5.c clcg4.o
	$(MPICC) $(LOCAL_CFLAGS) $(CFLAGS) $^ $(LIBS) -o $@

assignment4-5-debug: src/assignment4-5.c clcg4.o
	$(MPICC) $(DEBUG_CFLAGS) $(CFLAGS) $^ $(LIBS) -o $@

clcg4.o: src/clcg4.c src/clcg4.h
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXECUTABLES) clcg4.o
