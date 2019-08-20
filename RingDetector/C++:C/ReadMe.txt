程序接受A4纸样式的输入，输出为提取公章图片 *_S.jpg ，以及公章和签字图片 *_L.jpg，
主要借助了elsdc算法来检测圆弧，以及opencv库来处理图片。
其中elsdc算法源码（https://github.com/viorik/ELSDc），其中所需要的Lapack库文件以及编译完成在对应的文件夹lib下，linux需要自行编译）
opencv版本为4.1.0（opencv3.x应该也能运行）。
此外还需要c++编译器g++。
注意：虽然elsdc源码能在windows上编译，但本程序windows编译失败，暂时未找到解决方法。

配置步骤：
1. modify "./RingDetector/Makefile" .
修改 ./RingDetector/Makefile 

2. In "./RingDetector/" run "make clean", then run "make" to compile program.
在RingDetector目录下 先运行 "make clean" ，再运行 "make" 编译 产生可执行文件elsdc

3.the program is use as "./elsdc input_pic (output_path)".
运行示范如下 "./elsdc input_pic (output_path)"


备注：
1.如果需要得到圆参数可以根据下面信息
x1, y1, x2, y2,圆弧两个端点
cx, cy, 圆弧对应椭圆（圆）中点
ax, bx,长轴、短轴
theta, 椭圆角度（圆的角度为0）
ang_start, ang_end（圆弧起始角度、终点角度）

2.https://github.com/viorik/ELSDc 
github上elsdc原本代码配置相关问题：
只需要配置lacpack路径，在其对应makefile中加入lapack库的路径，再进行make。
其原本代码只支持pgm格式的输入，输出为多边形（直线段）和圆弧。

