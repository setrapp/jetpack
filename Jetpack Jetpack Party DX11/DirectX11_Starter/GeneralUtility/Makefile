all:
	g++ GUParseUtility.cpp -o GUParseUtility.o -c
	g++ GUMathUtility.cpp -o GUMathUtility.o -c
	#g++ -static-libgcc GUParseUtility.cpp -o GUParseUtility.o -c
	#g++ -static-libgcc GUMathUtility.cpp -o GUMathUtility.o -c
	ar -r libgu.a GUParseUtility.o
	ar -r libgu.a GUMathUtility.o
