2019.08.16
README.txt
Java版本 本质是用opencv处理输入输出并调用elsdc程序。
需要：
1.java-opencv 库文件在\Detection\lib目录下（不同系统可能不能通用，需要重新编译opencv文件）
IDEA：在配置的VM options中加入（路径改为本地java-opencv路径） -Djava.library.path=/Users/applezhj/Desktop/RingDetector/JAVA/Detection/lib/java-opencv
eclipse：。

2.编译elsdc程序。
所需的mac和windows的lacpack库文件在 “配置文件下”，linux需要自行编译。
windows可能还需要安装mingw，（gcc和make），可以参考 https://blog.csdn.net/z623838824/article/details/95884284 
修改makefile文件中LAPACK_LIB对应的路径，然后使用make命令编译。

3.将编译好的elsdc（linux、mac）或elsdc.exe（windows）放置工程目录下，或者在runElsdc函数中增加对应路径。