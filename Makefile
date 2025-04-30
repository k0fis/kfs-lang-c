OUT=out
FILES= ${OUT}/lexer.c ${OUT}/parser.c src/hashmap/hashmap.c \
		src/rt/expression.c src/rt/value.c src/rt/value_ops.c \
		src/rt/kfs_lang_env.c src/rt/named_value.c
CC= cc
CFLAGS= -g -Isrc -Isrc/rt -I${OUT}

test-regex: test/pg.c
	$(CC) $(CFLAGS) test/pg.c -o ${OUT}/tst
	${OUT}/tst
	rm -rf ${OUT}/tst*

test-kfs-lang: $(FILES) test/test.c ${OUT}
	$(CC) $(CFLAGS) $(FILES) test/test.c -o ${OUT}/kfsLang
	${OUT}/kfsLang

${OUT}/lexer.c: src/lexer.l ${OUT}
	flex src/lexer.l

${OUT}/parser.c: src/parser.y ${OUT} ${OUT}/lexer.c
	bison src/parser.y

${OUT}:
	mkdir -p ${OUT}

clean:
	rm -rf ${OUT}/*
