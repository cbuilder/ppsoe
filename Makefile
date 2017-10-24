all:
	gcc -O2 -Wall *.c -o ppsoe
debug:
	gcc -g -ggdb *.c -o ppsoe
clean:
	rm -rf *.o ppsoe
