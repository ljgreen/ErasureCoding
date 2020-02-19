LIB_NAME=parity
STATIC_NAME=lib$(LIB_NAME).a
SHARE_NAME=lib$(LIB_NAME).so

all: test static_library shared_library
 
test: main.o parity.o
	gcc -o test main.o parity.o -lJerasure

parity.o:  parity.h parity.c
	gcc -c parity.c

static_library: 
	gcc parity.c -o *.o -c
	ar -crv $(STATIC_NAME) *.o
 
shared_library:
	gcc -shared -fpic -o $(SHARE_NAME) parity.c;
 
clean:
	rm -rf *.o test
	rm -rf $(STATIC_NAME) $(SHARE_NAME)

