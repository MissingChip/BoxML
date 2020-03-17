
box_req = A/box.cpp A/box.h A/boxml.cpp A/boxml.h A/makefile

boxml_test: A/libboxml.a $(box_req) boxml_test.cpp
	g++ -o boxml_test boxml_test.cpp  -L./A -lboxml

bomhtml: A/libboxml.a $(box_req) bomhtml.cpp
	g++ -o bomhtml bomhtml.cpp  -L./A -lboxml

botex: A/libboxml.a $(box_req) botex.cpp
	g++ -o botex botex.cpp  -L./A -lboxml
	
A/libboxml.a : $(box_req)
	make -C ./A
