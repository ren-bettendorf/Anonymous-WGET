all: awget ss

awget: awget.c
	gcc -o awget awget.c
ss: ss.cpp
	g++ -o ss ss.cpp -pthread

clean:
	rm -f awget
	rm -f ss
