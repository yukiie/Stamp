#一张张复制图片太蠢了，写个程序，遍历目录下文件，依次显示*_S.jpg文件，然后根据交互输入，改名（不重名）到对应输出目录中

import os
import cv2
import shutil
sourceFloder="/Users/applezhj/workspace/pycharm/deepLearing/StampDB/tempData"
outdir="/Users/applezhj/workspace/pycharm/deepLearing/StampDB/Data"
outdir0="/Users/applezhj/workspace/pycharm/deepLearing/StampDB/Data/0"
outdir1="/Users/applezhj/workspace/pycharm/deepLearing/StampDB/Data/2"
# outdir3="/Users/applezhj/workspace/pycharm/deepLearing/StampDB/Data/3"
outdir_erro="/Users/applezhj/workspace/pycharm/deepLearing/StampDB/Data/-1"



def savePic(imgpath):
    im = cv2.imread(imgpath)
    cv2.imshow("classfy",im)
    inputnumber = cv2.waitKey()
    cv2.destroyAllWindows()
    # picClass = input("请输入图片所属类别：")
    print("图片:\t%s"%imgpath)
    print("分类结果:\t%s"%chr(inputnumber))
    targetPtah = os.path.join(outdir,chr(inputnumber))
    if not os.path.exists(targetPtah):
        targetPtah = outdir_erro
    print("保存路径\t%s"%targetPtah)
    temp = os.listdir(targetPtah)
    num = len(temp)
    newFilename = os.path.join(targetPtah,str(num)+".jpg")
    while(os.path.exists(newFilename)):
        num=num+1
        newFilename = os.path.join(targetPtah,str(num)+".jpg")
    shutil.move(imgpath, newFilename)

def handMovement(dirPath):
    filelist = os.listdir(dirPath)
    for file in filelist:
        fullFilePath = os.path.join(dirPath,file)
        if(os.path.isdir(fullFilePath)):
            handMovement(fullFilePath)
        elif file.endswith("_L.jpg"):
            savePic(fullFilePath)

def main():
    handMovement(sourceFloder)


if __name__ == '__main__':
    main()
