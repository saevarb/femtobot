
femtobot: obj bin main.o config_parsing.o  error_handling.o
	gcc -Wall obj/main.o obj/config_parsing.o obj/error_handling.o -o bin/femtobot
	
obj:

	if test -d obj; then echo obj exists; else mkdir obj; fi
bin:
	if test -d bin; then echo bin exists; else mkdir bin; fi

main.o: main.c main.h prototypes.h structs.h errors.h
	gcc -Wall -c main.c -o obj/main.o
	
config_parsing.o: config_parsing.c config_parsing.h structs.h errors.h
	gcc -Wall -c config_parsing.c -o obj/config_parsing.o
	
error_handling.o: error_handling.c error_handling.h errors.h
	gcc -Wall -c error_handling.c -o obj/error_handling.o



.PHONY: clean


clean:
	rm obj/*.o
	
