#Copyright Affonso Amendola 2018
#fflibvga
#fofonso's VGA library

LIBNAME = fflibvga
CC = tcc
LIBRARIAN = tlib
RM = rm -f

INCLUDE = include;D:\tc\include
LIB = D:\tc\lib
CCFLAGS = -c

all: $(LIBNAME).OBJ

$(LIBNAME).OBJ:
	$(CC) "-I$(INCLUDE) -L$(LIB) -o$(LIBNAME).OBJ $(CCFLAGS)" *.c
	tlib ".\$(LIBNAME).LIB + $(LIBNAME).OBJ"

exe:
	$(CC) "-I$(INCLUDE) -e$(LIBNAME) -L$(LIB)" *.c

clean:
	$(RM) *.OBJ *.EXE *.LOG *.BAT *.OBJ *.LIB *.AAA *.BAK

run:
	dosbox -conf ~/.dosbox/tcc.conf -c "$(LIBNAME)"