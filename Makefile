#
# UFRGS - Compiladores B - Marcelo Johann - 2023/1 - Etapa 3
#

etapa3: main.o lex.yy.o y.tab.o hash.o
	gcc main.o lex.yy.o y.tab.o hash.o -o etapa3
main.o: main.c
	gcc -c main.c
lex.yy.o: lex.yy.c
	gcc -c lex.yy.c
y.tab.o: y.tab.c
	gcc -c y.tab.c
hash.o: hash.c
	gcc -c hash.c
y.tab.c: parser.y
	yacc -d parser.y
lex.yy.c: scanner.l
	lex scanner.l

clean:
	rm lex.yy.c y.tab.c *.o etapa3
