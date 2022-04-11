CC = gcc
LIB_SO = libso_stdio.so
SOURCE = tema2.c
OBJECT = tema2.o

build:	$(OBJECT)
	$(CC) -shared -o $(LIB_SO) $^
$(OBJECT) : $(SOURCE)

clean:
	rm -f *.o $(LIB_SO) *.out