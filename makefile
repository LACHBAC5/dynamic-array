all: main.out test.out

main.out: main.o dynamic-array.o
	g++ -o $@ $^

test.out: test.o dynamic-array.o
	g++ -o $@ $^

%.o: %.cpp
	g++ -c -O3 $<