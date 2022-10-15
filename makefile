pictovectwave: lodepng.o vector.o
	g++ -std=c++11 -pthread -o ./pictovectwave ./pictovectwave.o ./lodepng/lodepng.o

vector.o: pictovectwave.cpp
	g++ -std=c++11 -pthread -c ./pictovectwave.cpp -o ./pictovectwave.o -DLINUX_BUILD

lodepng.o: ./lodepng/lodepng.cpp ./lodepng/lodepng.h
	g++ -std=c++11 -pthread -c ./lodepng/lodepng.cpp -o ./lodepng/lodepng.o

clean:
	rm *.o
