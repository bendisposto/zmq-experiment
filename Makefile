master: master.c hashmap.c mqhelper.c
	gcc -O3 -lzmq -o master master.c

worker: worker.c hashmap.c mqhelper.c queue.c graph.c sha.c zhelpers.h
	gcc -O3 -lzmq -o worker worker.c

all: master worker
	
clean:
	rm -f master worker