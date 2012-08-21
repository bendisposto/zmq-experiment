LIBRARIES=-lzmq -lczmq
WARN=-Wall -Wno-unused-function
MODE_D=-O0 -g -DDEBUG
MODE_R=-O3

master: master.c mqhelper.c zhelpers.h
	gcc ${MODE_R} -o master master.c ${LIBRARIES} ${WARN}

worker: worker.c hashmap.c mqhelper.c queue.c graph.c sha.c mqhelper.c zhelpers.h
	gcc ${MODE_R} -o worker worker.c ${LIBRARIES} ${WARN}

pok: savfile pok.c sha.c graph.c mqhelper.c zhelpers.h
#	gcc ${MODE_R} -o pok pok.c ${LIBRARIES} ${WARN}
	spld --static pok.c -o pok -lzmq -lczmq --cflag=-Wall,-Wno-unused-function

all: master worker pok savfile

savfile: root.pl
	sicstus -l root.pl --goal "save_program('root.sav'),halt."

	
clean:
	rm -f master worker pok root.sav
