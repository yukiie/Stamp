##
2019.08.15
create by zhj
目的：训练模型，对印章进行分类，并使用。
依赖库：torchvision，pytorch，numpy，（py-opencv ）

文件说明：
1.Train.py 训练模型
2.Test.py 测试模型
3.Use.py 使用模型
4.DataOperation.py 输入输出数据的一些转换
5.ImgAnnotation.py 显示图片，并根据输入移动图片至相关文件夹
6.SeprateTrainTest.py 根据随机数，随机将样本数据分为测试集和训练集
7.WriteLabelFile.py 将路径和label写入txt文件
8./StampDB/train.txt 训练图片路径和标签, /StampDB/test.txt  测试图片路径和标签
9./StampDB/train 训练图片目录, /StampDB/test 测试图片目录
10.***.pkl 训练好的网络模型
11.ServerTrain0.4.1.py pytorch1.x版本可忽略此文件，服务器上的版本是0.4.1，代码有点不一样，所以做了些改动。
12./训练好的模型 存放一些训练好的模型文件
13./StampDB/数据类别.xlsx 放类别相关的一些说明
14./StampDB/Data  /StampDB/tempData 手动分数据用的中间文件 可以删除