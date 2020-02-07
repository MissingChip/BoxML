
box_req = A/box.cpp A/box.h A/boxml.cpp A/boxml.h A/makefile

boxml_test: $(box_req) boxml_test.cpp
	make -C ./A
	g++ -o boxml_test boxml_test.cpp  -L./A -lboxml
