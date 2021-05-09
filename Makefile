mipcc: grammar.tab.o lex.yy.o symbol_table.o logging.o operators.o
	cc -g grammar.tab.o -o mipcc lex.yy.o symbol_table.o logging.o operators.o -lfl

grammar.tab.o: grammar.tab.h grammar.tab.c
	cc -g -c grammar.tab.c

lex.yy.o: grammar.tab.h lex.yy.c
	cc -g -c lex.yy.c

symbol_table.o: symbol_table.c
	cc -g -c symbol_table.c

logging.o: logging.c
	cc -g -c logging.c

operators.o: operators.c
	cc -g -c operators.c


lex.yy.c: tokens.l
	flex tokens.l

grammar.tab.h: grammar.y
	bison -d grammar.y

clean:
	rm grammar.tab.c grammar.tab.o lex.yy.o logging.o operators.o symbol_table.o mipcc grammar.tab.h lex.yy.c grammar.output
