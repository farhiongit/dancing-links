CC				= clang
WARNINGS	= -Wall -pedantic
#For debuging, use DEBUG instead of COMPILE
COMPILE		= -pipe
OPTIM			= -g
#OPTIM			= -O3
#PROF			= -pg
#For profiling, run executable, then "gprof <executable> gmon.out"
#PROC_OPT        = -march=i686
LD_OPT		= -s
CFLAGS  = $(OPTIM) $(PROF) $(WARNINGS) $(COMPILE) $(PROC_OPT)

SRCS    = dancing_links.c
HDRS    = dancing_links.h
OBJS    = $(SRCS:.c=.o)
LIB     = libdlx.a

NM_OPT  =--extern-only --defined-only -Col

all : run doc

$(OBJS) : $(SRCS) $(HDRS)

$(LIB) : $(OBJS)
	@cloc --quiet --by-file-by-lang --force-lang="C",m $(SRCS) $(HDRS)
	@rm -f "$(LIB)"
	ar rcs "$(LIB)" "$(OBJS)"
	@nm $(NM_OPT) "$(LIB)"

.PHONY: clean
clean:
	rm -f $(OBJS) $(LIB) core *~

dancing_links_test: $(LIB) main.c
	$(CC) $(CFLAGS) main.c $(LIB) -o dancing_links_test
	#./dancing_links_test
	valgrind --leak-check=full --show-leak-kinds=all ./dancing_links_test
	gprof ./dancing_links_test gmon.out

.PHONY: run
run: dancing_links_test

.PHONY: doc
doc: dancing_links.pdf

dancing_links.pdf: $(SRCS) $(HDRS) dancing_links.doxygen
	doxygen dancing_links.doxygen
	(cd doxygen/latex ; make > /dev/null ; mv refman.pdf ../../dancing_links.pdf ; cd - ; rm -rf doxygen/latex)
