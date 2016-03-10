LIBRARIES = -lm
CC=clang

.PHONY: test

all: icpc2015A.out

icpc2015A.out: icpc2015A.c
	$(CC) $^ -o $@ $(LIBRARIES)

test: icpc2015A.out
	./test.sh

clean:
	rm -f icpc2015A.out
