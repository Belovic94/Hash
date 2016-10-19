C = gcc
C_FLAGS = -g -O2 -std=c99 -Wall -Wconversion -Wno-sign-conversion -Wbad-function-cast -Wshadow -Wpointer-arith -Wunreachable-code -Wformat=2 -Werror
DEFAULT = TPHash
OBJ_HASH = hash.o
OBJ_TESTING  = testing.o
OBJS =  hash.o testing.o
VALGRIND = valgrind
V_FLAGS = --leak-check=full --track-origins=yes --show-reachable=yes

all: clean $(DEFAULT)

$(DEFAULT): $(OBJS)
		$(CC) $(C_FLAGS) main.c $(OBJS) pruebas_catedra.c  -o $(DEFAULT)

$(OBJ_HASH): hash.c hash.h
		$(CC) $(C_FLAGS) hash.c -c

$(OBJ_TESTING): testing.c testing.h
		$(CC) $(C_FLAGS) testing.c -c

run: $(DEFAULT)
		$(VALGRIND) $(V_FLAGS) ./$(DEFAULT)

clean_obj:
		rm -f *.o

clean: clean_obj
		rm -f $(DEFAULT)
