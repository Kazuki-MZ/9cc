CFLAGS=-std=c11 -g -static

9cc: 9cc.c
	docker run -it -v ${CURDIR}:/9cc -w /9cc compilerbook cc -o 9cc 9cc.c

test: 9cc
	docker run -it -v ${CURDIR}:/9cc -w /9cc compilerbook ./test.sh

clean:
	rm -f 9cc *.o *~ tmp*

.PHONY: test clean