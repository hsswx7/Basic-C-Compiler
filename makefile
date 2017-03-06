comp: main.o scanner.o parser.o buildTree.o semantics.o stack.o
	gcc -o comp  main.o scanner.o parser.o buildTree.o semantics.o stack.o
main.o: main.c scanner.h token.h parser.h semantics.h
	gcc -g -c main.c
scanner.o: scanner.c scanner.h token.h parser.h
	gcc -g -c scanner.c
parser.o: parser.c parser.h token.h scanner.h buildTree.h semantics.h
	gcc -g -c parser.c
buildTree.o: buildTree.c buildTree.h parser.h token.h scanner.h
	gcc -g -c buildTree.c
semantics.o: semantics.c semantics.h token.h parser.h buildTree.h scanner.h stack.h
	gcc -g -c semantics.c
stack.o: stack.c stack.h token.h
	gcc -g -c stack.c
make clean:
	rm -rf *.o
