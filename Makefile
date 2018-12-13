#Copyright Affonso Amendola 2018
#libVGAdos

LIBNAME = libVGA87
CC = tcc
LIBRARIAN = tlib
RM = rm -f

INCLUDE = include;D:\tc\include
LIB = D:\tc\lib
CCFLAGS = -c

all: $(LIBNAME).o

$(LIBNAME).o:
	$(CC) "-I$(INCLUDE) -L$(LIB) -o$(LIBNAME).o $(CCFLAGS)" *.c
	tlib ".\$(LIBNAME).LIB + $(LIBNAME).o"

clean:
	$(RM) *.OBJ *.EXE *.LOG *.BAT *.O *.LIB