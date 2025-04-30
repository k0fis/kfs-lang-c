OUT=../out
FILES= ${OUT}/lexer.c ${OUT}/parser.c hashmap/hashmap.c \
		rt/expression.c rt/value.c rt/value_ops.c rt/kfs_lang_env.c \
		rt/named_value.c
CC= cc
CFLAGS= -g -I. -Irt -I${OUT}

tst: ../test/pg.c
	$(CC) $(CFLAGS) ../test/pg.c -o ${OUT}/tst
	${OUT}/tst
	rm -rf ${OUT}/tst*

test: $(FILES) ../test/test.c ${OUT}
	$(CC) $(CFLAGS) $(FILES) test.c -o ${OUT}/kfsLang
	${OUT}/kfsLang

${OUT}/lexer.c: lexer.l ${OUT}
	flex -s lexer.l

${OUT}/parser.c: parser.y ${OUT} ${OUT}/lexer.c
	bison parser.y

${OUT}:
	mkdir -p ../out

clean:
	rm -rf ${OUT}/*
