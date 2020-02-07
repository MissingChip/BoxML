

libboxml: boxml.o box.o
	ar -rcs libboxml.a boxml.o box.o

boxml.o: boxml.cpp boxml.h
	g++ -c boxml.cpp -o boxml.o

box.o: box.cpp box.h
	g++ -c box.cpp -o box.o
