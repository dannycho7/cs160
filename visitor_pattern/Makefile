# Makefile for CS160 Project 1

CC		= gcc -g
CXX		= g++ -std=c++11 -g

TARGET	= tree
SRCS	= main.cpp print.cpp sum.cpp max.cpp
HDRS	= tree.hpp visitor.hpp
OBJS	= $(subst .cpp,.o,$(SRCS))
RMFILES	= $(OBJS) $(TARGET)

$(TARGET): $(SRCS) $(HDRS) $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS)

main.o: main.cpp $(HDRS)
	$(CXX) -o $@ -c $<

%.o: %.cpp %.hpp $(HDRS)
	$(CXX) -o $@ -c $<

.PHONY: run
run: $(TARGET)
	./$(TARGET) < example.tree
	./$(TARGET) < example1.tree
	./$(TARGET) < example2.tree
	./$(TARGET) < example3.tree
	./$(TARGET) < example4.tree
	./$(TARGET) < example5.tree
	./$(TARGET) < example6.tree
	./$(TARGET) < example7.tree
	./$(TARGET) < example8.tree
	./$(TARGET) < example9.tree
	./$(TARGET) < example10.tree
	./$(TARGET) < example11.tree
	./$(TARGET) < example12.tree
	./$(TARGET) < example13.tree
	./$(TARGET) < example14.tree
	./$(TARGET) < example15.tree
	./$(TARGET) < example16.tree
	./$(TARGET) < example17.tree
	./$(TARGET) < example18.tree
	./$(TARGET) < example19.tree
	./$(TARGET) < example20.tree
	./$(TARGET) < example21.tree
	./$(TARGET) < example22.tree
	./$(TARGET) < example23.tree
	./$(TARGET) < example24.tree
	./$(TARGET) < example25.tree

.PHONY: clean
clean:
	rm -f $(RMFILES)
