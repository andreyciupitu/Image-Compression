build:
	gcc -g -Wall BMP_read.c quadtree.c main.c -o quadtree -lm
clean:
	rm -f quadtree
