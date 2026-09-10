# CC = gcc
# CFLAGS = -Wall -g -fsanitize=address
# LDFLAGS = -fsanitize=address

# TaskB: main.c array.c array.h
# 	$(CC) $(CFLAGS) main.c array.c -o TaskB $(LDFLAGS)

# #clean:
# 	rm -f TaskB

CC = gcc
CFLAGS = -Wall -g

TaskB: main.c array.c array.h
	$(CC) $(CFLAGS) main.c array.c -o TaskB

clean:
	rm -f TaskB