
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
	objdump -DrwC -Mintel generated.o

link:
	ld generated.o -o gn
	

exec: link
	./gn

debug: link
	gdb gn

clean:
	rm -f generated.o