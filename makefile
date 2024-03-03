default:
	g++ -o bin main.cpp
	./examples.sh

clean:
		rm -f bin
		rm -f ex*.pdf
