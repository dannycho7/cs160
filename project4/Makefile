BISON	= bison
FLEX	= flex
CC		= gcc
CXX		= g++
FLAGS   = # add the -g flag to compile with debugging output for gdb
TARGET	= lang
BISONFLAGS	= -d -v -Wall

OBJS = ast.o parser.o lexer.o main.o

# set bison and flex paths for OS X if
# Homebrew is installed
ifeq ($(shell uname),Darwin)
ifneq ($(wildcard /usr/local/opt/bison/bin/bison),)
BISON	= /usr/local/opt/bison/bin/bison
endif
ifneq ($(wildcard /usr/local/opt/flex/bin/flex),)
FLEX	= /usr/local/opt/flex/bin/flex
endif
endif

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS)

lexer.o: lexer.l parser.y
	$(FLEX) -o lexer.cpp lexer.l
	$(CXX) $(FLAGS) -c -o lexer.o lexer.cpp

parser.o: parser.y
	$(BISON) $(BISONFLAGS) -o parser.cpp parser.y
	$(CXX) $(FLAGS) -c -o parser.o parser.cpp

genast: ast.cpp

ast.cpp:
	python3 genast.py -i lang.def -o ast

ast.o: ast.cpp
	$(CXX) $(FLAGS) -c -o ast.o ast.cpp

main.o: main.cpp
	$(CXX) $(FLAGS) -c -o main.o main.cpp

.PHONY: run
run: $(TARGET)
	@python3 runtests.py -v

.PHONY: diff
diff: $(TARGET)
	python3 runtests.py | diff - output.txt

.PHONY: clean
clean:
	rm -f *.o *~ lexer.cpp parser.cpp parser.hpp ast.cpp ast.hpp parser.output $(TARGET)
