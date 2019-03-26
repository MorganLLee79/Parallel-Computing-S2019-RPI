CFLAGS = -Wall -Werror -O3 -Isrc
LIBS = -pthread

EXECUTABLES = assignment4-5
all: $(EXECUTABLES)

assignment4-5: src/assignment4-5.c clcg4.o
	mpicc $(CFLAGS) $(LIBS) $^ -o $@

clcg4.o: src/clcg4.c src/clcg4.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXECUTABLES) clcg4.o
