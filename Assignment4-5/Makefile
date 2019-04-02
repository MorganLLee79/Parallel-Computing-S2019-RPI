LOCAL_CFLAGS = -Isrc
DEBUG_CFLAGS = $(LOCAL_CFLAGS) -g -Og -DDEBUG_MODE
LIBS = -lpthread

ifeq ($(HOSTNAME),q.ccni.rpi.edu)
	LOCAL_CFLAGS += -O5
else
	LOCAL_CFLAGS += -Wall -O3 -DROW_LENGTH=1024
endif

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
