MPICXX ?= mpic++

CXXFLAGS += -O3
LDFLAGS += -lpthread -lzoltan -lparmetis -lmetis

all: project.out

%.o: src/%.cpp
	$(MPICXX) $^ -c $(CXXFLAGS) $(CPPFLAGS) -o $@

project.out: data-structures.o pthread-wrappers.o project.o
	$(MPICXX) $^ $(LDFLAGS) $(LDLIBS) -o $@

clean:
	rm -f *.o *.out
