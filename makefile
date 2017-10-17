all: awget ss

awget: awget.cpp
	g++ -o awget awget.cpp
ss: ss.cpp
	g++ -o ss ss.cpp -pthread

clean:
	rm -f awget
	rm -f ss
