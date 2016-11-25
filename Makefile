NAME = draw

test: main.o draw.o
	g++ -Wall -g main.o draw.o -o $(NAME).out -std=c++11 -lglfw -lGLEW -lGL -lGLU -lfltk -lglut -lfltk_gl

draw.o: draw.h draw.cpp
	g++ -Wall -c -g draw.cpp -lglut -std=c++11 -lGL 

main.o: main.cpp
	g++ -c -g main.cpp -std=c++11 

clean:
	rm *.o *.out
