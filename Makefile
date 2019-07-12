all: main

main:
	gcc main.c json.c -lm

clean:
	rm -rf a.out