CC       = gcc -g
LEX      = flex -i -I 
INC      = -I../pl0/include 

all: labLexer-2 labLexer-2m

labLexer-2:	./grammar/relop.lex
	$(LEX)  ./grammar/relop.lex
	$(CC)  $(INC) -DLEXERGEN lex.yy.c ./src/labLexer-2.c -o labLexer-2 -ll

labLexer-2m:   
	$(CC)  $(INC) ./src/labLexer-2.c -o labLexer-2m -ll

clean:
	rm -f *.BAK *.o core *~* *.a 
	rm -f *.tab.h *.tab.c
	rm -f labLexer-2 labLexer-2m *.out