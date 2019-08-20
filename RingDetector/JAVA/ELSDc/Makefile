cc=gcc

#lapack Lib
LAPACK_LIB=-I/Users/applezhj/Desktop/RingDetector/configure/lib

elsdc: main.c pgm.c svg.c elsdc.c gauss.c curve_grow.c polygon.c ring.c ellipse_fit.c rectangle.c iterator.c image.c lapack_wrapper.c misc.c 
	$(cc)  -o $@ $^ $(LAPACK_LIB) -llapack -lm

clean: 
	rm elsdc