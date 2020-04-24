FLAGS =	-std=c99 -w

all:	exe

exe:	proj3.o	
	gcc $(FLAGS) -o project3 proj3.o 

proj3.o:proj3.c
	gcc $(FLAGS) -c proj3.c	 

clean:
	rm *.o 

