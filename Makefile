OUT=out
FILES= ${OUT}/lexer.c ${OUT}/parser.c  \
		src/rt/expression.c src/rt/value.c src/rt/value_ops.c \
		src/rt/kfs_lang_env.c src/rt/kfs_dict.c
CC= cc
CFLAGS= -g -Isrc -Isrc/rt -I${OUT}

test-variables: ${OUT}
	$(CC) $(CFLAGS) $(FILES) -DDEBUG -DTRACE test/pg.c  -o ${OUT}/tst
	leaks -atExit -- ${OUT}/tst
	rm -rf ${OUT}/tst*

test-kfs-lang: ${OUT}
	$(CC) $(CFLAGS) $(FILES) -DDEBUG test/test.c -o ${OUT}/kfsLang
	${OUT}/kfsLang

${OUT}/lexer.c: src/lexer.l ${OUT}
	flex src/lexer.l

${OUT}/parser.c: src/parser.y ${OUT} ${OUT}/lexer.c
	bison src/parser.y

${OUT}:
	mkdir -p ${OUT}

clean:
	rm -rf ${OUT}/*
