
FILES = Lexer.c Parser.c Expression.c main.c hashmap/hashmap.c rt/value.c rt/value_ops.c rt/eval.c
CC = gcc
CFLAGS = -g -I . -I hashmap -I rt -I ll
# gcc-> -fsanitize=leak -llsan
# clang -  -fsanitize=address

test: $(FILES)
	$(CC) $(CFLAGS) $(FILES) -o ../out/kfsLang
	rm -rf *.o *~ Lexer.c Lexer.h Parser.c Parser.h
	../out/kfsLang

Lexer.c: Lexer.l
	flex Lexer.l

Parser.c: Parser.y Lexer.c
	bison Parser.y

clean:
	rm -rf *.o *~ Lexer.c Lexer.h Parser.c Parser.h ../out/*