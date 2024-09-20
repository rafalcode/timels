CC=gcc
CFLAGS=-g -Wall
SPECLIBS=-lcairo -lm
CJPGLIBS=-lm -lcairo -lcairo_jpg -ljpeg # cairp +jpg libs
SPECINC=-I/home/nutria/mylocal/include
SPECINC2=-I/usr/include/cairo
EXECUTABLES=grid2v grid2vs grid2vp gribb

# just two vertical bars 
# actually this is pretty good:
grid2v: grid2v.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS}
# establishing sections.
grid2vs: grid2vs.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS}
# percentage ... or 1 hundred
grid2vp: grid2vp.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS}
gribb: gribb.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS}

.PHONY: clean

clean:
	rm -f ${EXECUTABLES}
