# Compiler options
CPPOPT=-g -Og -D_DEBUG
CPPFLAGS=$(CPPOPT) -Wall -ansi -pedantic -Iinclude -std=c++11

# documents and scripts
DOCS=Tasks.txt
SCRS=

# headers and code sources with updated paths
HDRS=	include/defs.h \
		include/Iterator.h include/Scan.h include/Filter.h include/Sort.h include/Record.h src/VerifyOrder.h
SRCS=	src/defs.cpp src/Assert.cpp src/Test.cpp src/TestMain.cpp \
		src/Iterator.cpp src/Scan.cpp src/Filter.cpp src/Sort.cpp src/Record.cpp src/VerifyOrder.cpp

# compilation targets
OBJS=	defs.o Assert.o Test.o TestMain.o \
		Iterator.o Scan.o Filter.o Sort.o Record.o VerifyOrder.o

# default target
Test.exe : $(OBJS)
	@echo "Linking $@"
	g++ $(CPPFLAGS) -o Test.exe $(OBJS)

# To generate each object file
%.o : src/%.cpp
	@echo "Compiling $<"
	g++ $(CPPFLAGS) -c $< -o $@

# other rules
trace : Test.exe
	@date > trace
	./Test.exe >> trace
	@size -t Test.exe $(OBJS) | sort -r >> trace

list :
	echo Makefile $(HDRS) $(SRCS) $(DOCS) $(SCRS) > list

count : list
	@wc `cat list`

clean :
	@rm -f $(OBJS) Test.exe Test.exe.stackdump trace
