JS_INCLUDE = -I./include

JS = \
./bigint_operation.c \
./nistseedexpander.c \
./aes.c \
./bigint_struct.c \
./randombytes.c

all: main.o

main.o : main.c
	g++	-g $(JS_INCLUDE) $(JS) main.c -o test -DGPP
	# gcc	-g $(JS_INCLUDE) $(JS) main.c -o test

clean:
	rm	*.o	test