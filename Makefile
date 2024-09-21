all: build run

build:
	cc main.c -o vos -save-temps

run:
	./vos

repl:
	./vos repl