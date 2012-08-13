master: master.c hashmap.c mqhelper.c
	gcc -O3 -lzmq -o master master.c

worker: worker.c hashmap.c mqhelper.c queue.c graph.c sha.c zhelpers.h
	gcc -O3 -lzmq -lczmq -o worker worker.c

pok: pok.c  sha.c 
	gcc -O3 -lzmq -lczmq -o pok pok.c

all: master worker pok
	
clean:
	rm -f master worker pok