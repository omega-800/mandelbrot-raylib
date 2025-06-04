LIBS=$(pkg-config --cflags raylib) -lraylib \
		 -l pthread


all: 
	$(CC) src/*.c -std=c99 -o mandelbroetli-c -Wall -Wextra -Werror -fsanitize=address -g3 -lm $(LIBS)
clean: 
	rm mandelbroetli-c

