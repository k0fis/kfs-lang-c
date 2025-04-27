OUT=../out
FILES= main.c ${OUT}/lexer.c ${OUT}/parser.c hashmap/hashmap.c \
		rt/expression.c rt/value.c rt/value_ops.c rt/eval.c
CC= cc
CFLAGS= -g -I. -Irt -I${OUT}

test: $(FILES) ${OUT}
	$(CC) $(CFLAGS) $(FILES) -o ${OUT}/kfsLang
	${OUT}/kfsLang

${OUT}/lexer.c: lexer.l ${OUT}
	flex lexer.l

${OUT}/parser.c: parser.y ${OUT} ${OUT}/lexer.c
	bison parser.y

${OUT}:
	mkdir -p ../out

clean:
	rm -rf ${OUT}/*
