#before make,please replace the path of opencv and lapack
#replace the Opencv include path and lib path in new computer
#将路径换成本地opencv路径（保留-I）
OPENCV_INCLUDE = -I/usr/local/Cellar/opencv/4.1.0_2/include/opencv4
#将路径换成本地opencv路径（保留-L）
OPENCV_LIB = -L/usr/local/Cellar/opencv/4.1.0_2/lib

LIBS = -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_imgcodecs

#opencv4.x need c++11, opencv3.x do not need
#opencv4以上的版本需要使用c++11标准，opencv3.x直接用g++
cc = g++ --std=c++11
#cc = g++

#replace the lapack lib path in new computer
#将路径换成本地lapack库文件路径（保留-L）
#linux和mac的lib文件在/linux_mac-config/lib目录下
#windows的lib文件在\windows\lib目录下
LAPACK_LIB=-I/Users/applezhj/Desktop/RingDetector/linux_mac-config/lib

elsdc: main.cpp pgm.c svg.c elsdc.c gauss.c curve_grow.c polygon.c ring.c ellipse_fit.c rectangle.c iterator.c image.c lapack_wrapper.c misc.c 
	$(cc) $(OPENCV_INCLUDE) $(OPENCV_LIB) $(LIBS) $(LAPACK_LIB) -llapack -lm -o $@ $^

clean: 
	rm elsdc