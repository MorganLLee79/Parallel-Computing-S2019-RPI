CXX ?= bgxlc++
MPICXX ?= mpixlcxx

include $(ZOLTAN_INSTALL_DIR)/include/Makefile.export.zoltan

CXXFLAGS += -qlanglvl=extended0x -qlanglvl=noimplicitint $(ZOLTAN_INCLUDES) -I$(ZOLTAN_INSTALL_DIR)/include -O3
LDFLAGS += -qlanglvl=extended0x -lpthread $(ZOLTAN_LIBS)

all: project.out

objs: obj/data-structures.o obj/pthread-wrappers.o obj/project.o

obj/%.o: src/%.cpp
	$(MPICXX) $^ -c -g $(CXXFLAGS) $(CPPFLAGS) -o $@

project.out: obj/data-structures.o obj/pthread-wrappers.o obj/project.o
	$(MPICXX) $^ $(LDFLAGS) $(LDLIBS) -o $@
	@printf ''

clean:
	rm -f obj/*.o *.out
