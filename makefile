CC = gcc
LDFLAGS = -g -Wall -ldl
CFLAGS = -I ./include -g -Wall -c
EXEC = polash pola-i
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
VPATH = src
BINDIR = bin
LIBDIR = lib

all : $(EXEC)

$(EXEC) : $(OBJ) $(LIBDIR)/polalib.so
	$(CC) -o $(BINDIR)/$@ src/$@.o $(LDFLAGS) 

%.o : %.c
	$(CC) -o $@ $(CFLAGS) $<

$(LIBDIR)/polalib.so : src/polalib.o
	$(CC) -o $@ -shared $^ $(LDFLAGS)


clean:
	@ rm -f src/*.o

mrproper:
	@ cd $(BINDIR) && rm -f $(EXEC)

