#将训练集和测试集路径和标签写入txt文件
import os
TrainFile = "/Users/applezhj/workspace/pycharm/deepLearing/StampDB/train.txt"
TestFile = "/Users/applezhj/workspace/pycharm/deepLearing/StampDB/test.txt"

BasePath = "/Users/applezhj/workspace/pycharm/deepLearing/StampDB"
TrainPath = "/Users/applezhj/workspace/pycharm/deepLearing/StampDB/train"
TestPath = "/Users/applezhj/workspace/pycharm/deepLearing/StampDB/test"
#过滤类别
fliterClass = ["-1","1"]

#清空文件
if os.path.exists(TrainFile):
    os.remove(TrainFile)
if os.path.exists(TestFile):
    os.remove(TestFile)

def outFileLableIt(filePath,outPath,outfile,label):
    filelist = os.listdir(filePath)
    for file in filelist:
        fullPath = os.path.join(filePath,file)
        if os.path.isdir(fullPath):
            tempPath = os.path.join(outPath,file)
            outFileLableIt(fullPath,tempPath,outfile,label)
        elif file.endswith(".jpg"):
            if not file.startswith("."):
                tempPath = os.path.join(outPath,file)
                message = tempPath + "\t" + label + "\n"
                f = open(outfile, 'a+')
                f.write(message)
                f.close()



#train
labelList = os.listdir(TrainPath)
for label in labelList:
    if label in fliterClass:
        continue
    elif label.startswith("."):
        continue
    else:
        fullPath = os.path.join(TrainPath,label)
        outPath = os.path.join("/train",label)
        outFileLableIt(fullPath,outPath,TrainFile,label)

#test
labelList = os.listdir(TestPath)
for label in labelList:
    if label in fliterClass:
        continue
    elif label.startswith("."):
        continue
    else:
        fullPath = os.path.join(TestPath,label)
        outPath = os.path.join("/test",label)
        outFileLableIt(fullPath,outPath,TestFile,label)