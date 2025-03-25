executable:main.o sys.o
	gcc main.o sys.o -o executable

sys.o:sys.c sys.h
	gcc -c sys.c

main.o:main.c sys.h
	gcc -c main.c

clean:
	rm *.o