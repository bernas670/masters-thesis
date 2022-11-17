.PHONY = all

SRC = src
BIN = bin

# DIFF = ${SRC}/diff_fw
ENRG = ${SRC}/energy-read
METR = ${SRC}/metrics_fw
UTIL = ${SRC}/utils


# diff-all: diff cbor-txt

# energy-all: energy-read metrics

# diff:
# 	gcc ${DIFF}/*.c ${DIFF}/*.h ${UTIL}/files.c ${UTIL}/files.h -o ${BIN}/diff

# cbor-txt:
# 	stack exec -- ghc ${SRC}/cbor-txt.hs -package ghc-dump-util -o ${BIN}/cbor-txt
# 	rm ${SRC}/*.hi ${SRC}/*.o

energy-read:
	@mkdir -p ${BIN}
	@mkdir -p logs
	@gcc -O2 -w -o ${BIN}/energy-read ${ENRG}/*.c ${ENRG}/*.h ${UTIL}/files.c ${UTIL}/files.h ${UTIL}/macros.h ${UTIL}/cpu.h ${UTIL}/cpu.c -lm

metrics:
	@mkdir -p ${BIN}
	@mkdir -p logs
	@gcc -O2 -w -o ${BIN}/metrics ${METR}/*.c ${METR}/*.h ${UTIL}/files.c ${UTIL}/files.h ${UTIL}/macros.h ${UTIL}/cpu.h ${UTIL}/cpu.c

clean:
	rm ${BIN}/*