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
	cp bot.conf ../bin/
	cp bot.modules ../bin/

libmodule:	../obj/libmodule.o ../bin/
	gcc -shared -o ../bin/libmodule.so ../obj/libmodule.o
	
../obj/libmodule.o:
	gcc -fPIC -c module.c -o ../obj/libmodule.o
	
clean:
	rm -rf ../obj/*.o
