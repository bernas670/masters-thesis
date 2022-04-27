.PHONY = all

SRC = src
BIN = bin

DIFF = ${SRC}/diff_fw
ENRG = ${SRC}/energy-read
METR = ${SRC}/metrics_fw
UTIL = ${SRC}/utils


all: diff cbor-txt

diff:
	gcc ${DIFF}/*.c ${DIFF}/*.h ${UTIL}/files.c ${UTIL}/files.h -o ${BIN}/diff

cbor-txt:
	stack exec -- ghc ${SRC}/cbor-txt.hs -package ghc-dump-util -o ${BIN}/cbor-txt
	rm ${SRC}/*.hi ${SRC}/*.o

energy-read:
	gcc -O2 -Wall -o ${BIN}/energy-read ${ENRG}/*.c ${ENRG}/*.h ${UTIL}/files.c ${UTIL}/files.h -lm

metrics:
	gcc -O2 -Wall -o ${BIN}/energy-read ${METR}/*.c ${METR}/*.h ${UTIL}/files.c ${UTIL}/files.h -lm

clean:
	rm ${BIN}/*