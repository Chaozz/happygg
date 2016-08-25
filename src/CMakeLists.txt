project(src)

set(SOURCE_FILES
    defs.h
    fen.cpp
    fen.h
    hash.cpp
    hash.h
    move.cpp
    move.h
    movegenerator.cpp
    movegenerator.h
    pipe.cpp
    pipe.h
    position.cpp
    position.h
    search.cpp
    search.h
    ucci.cpp
    ucci.h
    zobrist.cpp
    zobrist.h)

add_library(src STATIC ${SOURCE_FILES})