dxlwrite: dxlwrite.o libdxl_alpha.so copy
	g++ -I/usr/local/include/dynamixel_sdk dxlwrite.o -o dxlwrite -ldxl_x64_cpp -lrt -L. -ldxl_alpha

dxlwrite.o: dxlwrite.cpp
	g++ -c $? -o $@

copy: libdxl_alpha.so
	sudo cp libdxl_alpha.so /usr/lib/

libdxl_alpha.so: libdxlalpha.o
	g++ -shared -o libdxl_alpha.so libdxlalpha.o -ldxl_x64_cpp -lrt

libdxlalpha.o: libdxlalpha.cpp libdxlalpha.h
	g++ -I/usr/local/include/dynamixel_sdk -I. -fpic -c libdxlalpha.cpp -o libdxlalpha.o
