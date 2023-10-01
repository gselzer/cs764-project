# Compiler options
CPPOPT=-g -Og -D_DEBUG
CPPFLAGS=$(CPPOPT) -Wall -ansi -pedantic -Iinclude -std=c++11

# documents and scripts
DOCS=Tasks.txt
SCRS=

# headers and code sources with updated paths
HDRS=	include/defs.h \
		include/Iterator.h include/Scan.h include/Filter.h include/Sort.h include/Record.h

SRCS=	src/defs.cpp src/Assert.cpp src/Test.cpp src/TestMain.cpp \
		src/Iterator.cpp src/Scan.cpp src/Filter.cpp src/Sort.cpp src/Record.cpp

# compilation targets
OBJS=	defs.o Assert.o Test.o TestMain.o \
		Iterator.o Scan.o Filter.o Sort.o Record.o

# RCS assists
REV=-q -f
MSG=no message

# default target
Test.exe : Makefile $(OBJS)
	g++ $(CPPFLAGS) -o Test.exe $(OBJS)

# New target for unit testing

test_program: TestMain.o $(filter-out Test.o, $(OBJS))
	g++ $(CPPFLAGS) -o test_program TestMain.o $(filter-out Test.o, $(OBJS))


trace : Test.exe Makefile
	@date > trace
	./Test.exe >> trace
	@size -t Test.exe $(OBJS) | sort -r >> trace

$(OBJS) : Makefile include/defs.h
Test.o : include/Iterator.h include/Scan.h include/Filter.h include/Sort.h include/Record.h
Iterator.o Scan.o Filter.o Sort.o : include/Iterator.h
Scan.o : include/Scan.h
Filter.o : include/Filter.h
Sort.o : include/Sort.h
Record.o : include/Record.h
TestMain.o : include/Iterator.h include/Scan.h include/Filter.h include/Sort.h include/Record.h

list : Makefile
	echo Makefile $(HDRS) $(SRCS) $(DOCS) $(SCRS) > list
count : list
	@wc `cat list`

ci :
	ci $(REV) -m"$(MSG)" $(HDRS) $(SRCS) $(DOCS) $(SCRS)
	ci -l $(REV) -m"$(MSG)" Makefile
co :
	co $(REV) -l $(HDRS) $(SRCS) $(DOCS) $(SCRS)

clean :
	@rm -f $(OBJS) Test.exe Test.exe.stackdump trace
