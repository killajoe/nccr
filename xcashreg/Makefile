# Makefile
# Aufruf:  make <target> 
# <target> = clean, all

all:	xcashreg main.o
.PHONY : all

clean:
	-rm -f xcashreg *.o *~
	@echo "clean-up done"
.PHONY : clean

xcashreg: main.o
	gcc -Wall -O2 -lXft -o xcashreg main.o

main.o: main.c
	gcc -c -Wall main.c

