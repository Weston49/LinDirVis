default:
	g++ -std=c++11 -o bin main.cpp
	./examples.sh

clean:
		rm -f bin
		rm -f ex*.pdf
