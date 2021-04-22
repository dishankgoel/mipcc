cmips: grammar.tab.o lex.yy.o symbol_table.o error_reporting.o
	cc -g grammar.tab.o -o cmips lex.yy.o symbol_table.o error_reporting.o -lfl

grammar.tab.o: grammar.tab.h grammar.tab.c
	cc -g -c grammar.tab.c

lex.yy.o: grammar.tab.h lex.yy.c
	cc -g -c lex.yy.c

symbol_table.o: symbol_table.c
	cc -g -c symbol_table.c

error_reporting.o: error_reporting.c
	cc -g -c error_reporting.c

lex.yy.c: tokens.l
	flex tokens.l

grammar.tab.h: grammar.y
	bison -d grammar.y

clean:
	rm grammar.tab.c grammar.tab.o lex.yy.o error_reporting.o symbol_table.o cmips grammar.tab.h lex.yy.c grammar.output
