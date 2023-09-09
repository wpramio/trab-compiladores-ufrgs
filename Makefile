#
# UFRGS - Compiladores B - Marcelo Johann - 2023/1 - Etapa 6
#

etapa6: main.o lex.yy.o y.tab.o hash.o ast.o semantic.o tac.o asm.o
	gcc main.o lex.yy.o y.tab.o hash.o ast.o semantic.o tac.o asm.o -o etapa6
main.o: main.c
	gcc -c main.c
lex.yy.o: lex.yy.c
	gcc -c lex.yy.c
y.tab.o: y.tab.c
	gcc -c y.tab.c
hash.o: hash.c
	gcc -c hash.c
ast.o: ast.c
	gcc -c ast.c
semantic.o: semantic.c
	gcc -c semantic.c
tac.o: tac.c
	gcc -c tac.c
asm.o: asm.c
	gcc -c asm.c
y.tab.c: parser.y
	yacc -d parser.y
lex.yy.c: scanner.l
	lex scanner.l

clean:
	rm lex.yy.c y.tab.c *.o etapa6
