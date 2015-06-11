BIN = happygg

CC = g++
CFLAGS = -O3 -Wall
OBJECTS = fen.o happygg.o hash.o move.o movegenerator.o pipe.o position.o search.o ucci.o zobrist.o

happygg: ${OBJECTS}
	${CC} ${CFLAGS} -o $@ ${OBJECTS}

move.o: move.h defs.h
	${CC} -c move.cpp	
happygg.o: defs.h pipe.h ucci.h hash.h search.h
	${CC} -c happygg.cpp
hash.o: hash.h
	${CC} -c hash.cpp
movegenerator.o: movegenerator.h search.h
	${CC} -c movegenerator.cpp
pipe.o: pipe.h
	${CC} -c pipe.cpp
position.o: position.h move.h defs.h fen.h
	${CC} -c position.cpp
search.o: search.h defs.h movegenerator.h fen.h ucci.h
	${CC} -c search.cpp
ucci.o: pipe.h ucci.h
	${CC} -c ucci.cpp
zobrist.o: zobrist.h
	${CC} -c zobrist.cpp
fen.o: fen.h defs.h move.h
	${CC} -c fen.cpp

.PHONY: clean
clean:
	${RM} ${BIN} *.o