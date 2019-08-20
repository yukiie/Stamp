#随机分训练和测试图片
import random,os
import shutil

SourceFolder = "/Users/applezhj/workspace/pycharm/deepLearing/StampDB/Data/1"
TrainFolder = "/Users/applezhj/workspace/pycharm/deepLearing/StampDB/Data/train/1"
TestFolder = "/Users/applezhj/workspace/pycharm/deepLearing/StampDB/Data/test/1"
thre = 1

filelist = os.listdir(SourceFolder)
for file in filelist:
    temp = random.randint(0, 9)
    path0=os.path.join(SourceFolder,file)
    # print(path0)
    path1=os.path.join(TrainFolder,file)
    # print(path1)
    path2=os.path.join(TestFolder,file)
    if temp <= thre:
        shutil.move(path0, path2)
    else:
        shutil.move(path0, path1)



