
CXX = clang++
CXXFLAGS = -O2 -I./source/ -I./external/ -Wno-sizeof-array-argument
EXECUTABLE := bin/asev

SOURCES := $(shell find source/ -iname "*.cpp")


all: build

build:
	$(CXX) $(SOURCES) $(CXXFLAGS) -o $(EXECUTABLE)

run:
	./$(EXECUTABLE)

dump:
	objdump -drwC -Mintel generated.o

clean:
	rm -f generated.o