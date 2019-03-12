fetch:
	git clone https://github.com/tfussell/xlnt.git
	mkdir xlnt/build
	cd xlnt/build
	cmake ..
	make

all:
	g++ -std=c++14 xlsx2csv.cpp -o xlsx2csv -I ./xlnt/include ./xlnt/source/libxlnt.a
