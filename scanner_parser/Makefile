CC		= gcc
CXX		= g++
CXXFLAGS	= -g -std=c++11 -Wall

all:
	$(CXX) $(CXXFLAGS) helpers.cpp calculator.cpp -o calculator main.cpp

.PHONY: test
test: $(TARGET)
	ruby test/test.rb

.PHONY: clean
clean:
	rm -f *.o *~ $(TARGET)
	rm -rf *.dSYM
