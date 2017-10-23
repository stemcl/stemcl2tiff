CC = gcc -std=gnu99 -O3
DEL = rm

# define libraries 
MLIBS = tiff.o
WLIBS = tiff.o -L /usr/local/lib64 -ltiff -lm

stemcl2tiff: stemcl2tiff.c ${MLIBS}
	${CC} -o stemcl2tiff stemcl2tiff.c ${WLIBS}$

tiff.o: tiff.c
	${CC} -c tiff.c
