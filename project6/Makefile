BISON	= bison -d -v
FLEX	= flex
CC		= gcc
CXX		= g++
OFLAGS  = -std=c++11
FLAGS   = -Ofast # add the -g flag to compile with debugging output for gdb
TARGET	= lang

OBJS = ast.o parser.o lexer.o typecheck.o codegen.o main.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OFLAGS) -o $(TARGET) $(OBJS)

lexer.o: lexer.l
	$(FLEX) -o lexer.cpp lexer.l
	$(CXX) $(FLAGS) -c -o lexer.o lexer.cpp

parser.o: parser.y
	$(BISON) -o parser.cpp parser.y
	$(CXX) $(OFLAGS) $(FLAGS) -c -o parser.o parser.cpp

genast: ast.cpp

ast.cpp:
	python3 genast.py -i lang.def -o ast

ast.o: ast.cpp
	$(CXX) $(OFLAGS) $(FLAGS) -c -o ast.o ast.cpp
	
typecheck.o: typecheck.cpp typecheck.hpp
	$(CXX) $(OFLAGS) $(FLAGS) -c -o typecheck.o typecheck.cpp

codegen.o: codegeneration.cpp codegeneration.hpp
	$(CXX) $(OFLAGS) $(FLAGS) -c -o codegen.o codegeneration.cpp

main.o: main.cpp
	$(CXX) $(OFLAGS) $(FLAGS) -c -o main.o main.cpp

.PHONY: run
run: $(TARGET)
	@python3 runtests.py

.PHONY: diff
diff: $(TARGET)
	python3 runtests.py | diff - output.txt

test: $(TARGET) test.lang
	./$(TARGET) < test.lang > code.s
ifeq ($(shell uname), Darwin)
	gcc -Wl,-no_pie -m32 -o test tester.c code.s
else
	gcc -m32 -o test tester.c code.s
endif
	./test

.PHONY: clean
clean:
	rm -f *.o *~ lexer.cpp parser.cpp parser.hpp ast.cpp ast.hpp parser.output $(TARGET) test code.s
	rm -f tests/*.s tests/*.c
