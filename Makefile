all: compile

compile:
	gcc -o project project.c

clean:
	rm project
