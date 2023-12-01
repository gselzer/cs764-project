# Compiler options
CPPOPT=-g -Og -D_DEBUG
CPPFLAGS=$(CPPOPT) -Wall -ansi -pedantic -Iinclude -std=c++11

# documents and scripts
DOCS=Tasks.txt
SCRS=

# headers and code sources with updated paths
HDRS=	include/defs.h \
		include/Iterator.h include/Scan.h include/Filter.h include/Sort.h include/Record.h include/Run.h include/VerifyOrder.h include/LoserTree.h include/ExternalMergeSort.h include/VerifyContent.h
SRCS=	src/defs.cpp src/Assert.cpp src/Test.cpp src/Main.cpp \
		src/Iterator.cpp src/Scan.cpp src/Filter.cpp src/Sort.cpp src/Record.cpp src/Run.cpp src/VerifyOrder.cpp src/LoserTree.cpp src/ExternalMergeSort.cpp src/VerifyContent.cpp

# compilation targets
OBJS=	defs.o Assert.o Test.o Main.o \
		Iterator.o Scan.o Filter.o Sort.o Record.o Run.o VerifyOrder.o LoserTree.o ExternalMergeSort.o VerifyContent.o

# default target
sort : $(OBJS)
	@echo "Linking $@"
	g++ $(CPPFLAGS) -o sort $(OBJS)

# To generate each object file
%.o : src/%.cpp
	@echo "Compiling $<"
	g++ $(CPPFLAGS) -c $< -o $@

# other rules
trace : sort
	@date > trace
	./sort >> trace
	@size -t sort $(OBJS) | sort -r >> trace

list :
	echo Makefile $(HDRS) $(SRCS) $(DOCS) $(SCRS) > list

count : list
	@wc `cat list`

clean :
	@rm -f $(OBJS) sort sort.stackdump trace
