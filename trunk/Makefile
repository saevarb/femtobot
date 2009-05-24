../bin/ircbot:  ../obj/ircbot.o ../obj/botfunctions.o ../bin/
	gcc -ldl -Wall -ggdb3 ../obj/ircbot.o ../obj/botfunctions.o -o ../bin/ircbot

../obj/ircbot.o: ircbot.c includes.h prototypes.h botstructs.h ../obj/
	gcc -Wall -ggdb3 ircbot.c -c -o ../obj/ircbot.o
	
	
../obj/botfunctions.o: botfunctions.c includes.h botstructs.h ../obj/
	gcc -Wall -ggdb3 botfunctions.c -ldl -c -o ../obj/botfunctions.o

../obj/:
	mkdir ../obj
	
../bin/:
	mkdir ../bin

clean:
	rm -rf ../obj/*.o
