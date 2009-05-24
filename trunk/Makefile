ircbot:  ircbot.o botfunctions.o
	gcc -ldl -Wall -ggdb3 ircbot.o botfunctions.o -o ircbot

ircbot.o: ircbot.c includes.h prototypes.h botstructs.h
	gcc -Wall -ggdb3 ircbot.c -c
	
	
botfunctions.o: botfunctions.c includes.h botstructs.h
	gcc -Wall -ggdb3 botfunctions.c -ldl -c

clean:
	rm -rf *.o	
