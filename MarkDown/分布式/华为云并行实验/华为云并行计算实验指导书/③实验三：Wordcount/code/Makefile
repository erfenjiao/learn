CC = mpic++
CCFLAGS = -O2 -fopenmp
LDFLAGS = -lopenblas

all:  wordcount

wordcount: wordcount.cpp
	${CC} ${CCFLAGS} wordcount.cpp -o wordcount ${LDFLAGS}

clean:
	rm  wordcount
