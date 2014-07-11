all: a b c

a: cglib.o color.o a.o
	gcc -o a cglib.o color.o a.o -lm -lX11

b: cglib.o color.o b.o
	gcc -o b cglib.o color.o b.o -lm -lX11

c: cglib.o color.o c.o
	gcc -o c cglib.o color.o c.o -lm -lX11

color.o: color.c color.h
	gcc -o color.o -c color.c 

cglib.o: cglib.c cglib.h
	gcc -o cglib.o -c cglib.c -lm -lX11

a.o: a.c cglib.h
	gcc -o a.o -c a.c

b.o: b.c cglib.h
	gcc -o b.o -c b.c

c.o: c.c cglib.h
	gcc -o c.o -c c.c