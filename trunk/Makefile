
femtobot: obj bin main.o config_parsing.o  error_handling.o module_support.o connect.o
	gcc -ldl -ggdb3 -Wall obj/main.o obj/config_parsing.o obj/error_handling.o obj/module_support.o obj/connect.o -o bin/femtobot
	
obj:

	if test -d obj; then echo obj exists; else mkdir obj; fi
bin:
	if test -d bin; then echo bin exists; else mkdir bin; fi

main.o: main.c main.h prototypes.h structs.h errors.h
	gcc -ggdb3 -Wall -c main.c -o obj/main.o
	
connect.o: connect.c connect.h structs.h errors.h
	gcc -ggdb3 -Wall -c connect.c -o obj/connect.o
	
config_parsing.o: config_parsing.c config_parsing.h structs.h errors.h
	gcc -ggdb3 -Wall -c config_parsing.c -o obj/config_parsing.o
	
error_handling.o: error_handling.c error_handling.h errors.h
	gcc -ggdb3 -Wall -c error_handling.c -o obj/error_handling.o

module_support.o: module_support.h errors.h structs.h
	gcc -ggdb3 -Wall -c module_support.c -o obj/module_support.o
	
testmodules: testmodule1.c testmodule2.c
	gcc -ggdb3 -fPIC -c -Wall testmodule1.c -o testmodule1.o
	gcc -ggdb3 -fPIC -c -Wall testmodule2.c -o testmodule2.o
	gcc -ggdb3 -shared testmodule1.o -o testmodule1.so
	gcc -ggdb3 -shared testmodule2.o -o testmodule2.so


.PHONY: clean


clean:
	rm obj/*.o
	
