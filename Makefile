

CFLAGS =  -std=c99 -g

LDFLAGS = -lpthread -lm



.PHONY: all
all:  a2a a2b a2c a2d

	
a2a: a2a.c timing.o
	gcc ${CFLAGS} ${LDFLAGS} timing.o a2a.c -o a2a
	
a2b: a2b.c  
	gcc ${CFLAGS} ${LDFLAGS} a2b.c -o a2b
	
a2c: a2c.c  
	gcc ${CFLAGS} ${LDFLAGS} a2c.c -o a2c
		
a2d: a2d.c  
	gcc ${CFLAGS} ${LDFLAGS} a2d.c -o a2d
	

timing.o:timing.c timing.h
	gcc ${CFLAGS} -c timing.c

	
.PHONY: clean
clean:
	rm -rf   a2a a2b a2c a2d timing.o  *.dSYM

