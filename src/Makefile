
FILES = ../out/lexer.c ../out/parser.c rt/expression.c main.c hashmap/hashmap.c rt/value.c rt/value_ops.c rt/eval.c
CC = gcc
CFLAGS = -g -I . -I hashmap -I rt -I ll -I../out
# gcc-> -fsanitize=leak -llsan
# clang -  -fsanitize=address

test: $(FILES)
	mkdir -p ../out
	$(CC) $(CFLAGS) $(FILES) -o ../out/kfsLang
	rm -rf *.o *~ Lexer.c Lexer.h Parser.c Parser.h
	../out/kfsLang

lexer.c: lexer.l
	flex lexer.l

parser.c: parser.y lexer.c
	bison parser.y

clean:
	rm -rf *.o *~ lexer.c lexer.h parser.c parser.h ../out/*
