"""
数据的一些操作
zhj 2019.8.13
"""
import torch
import numpy as np
import cv2
import torch.utils.data as Data

class MyDataset(torch.utils.data.Dataset):  # 创建自己的类：MyDataset,这个类是继承的torch.utils.data.Dataset
    #从文本读图片和label
    def __init__(self, root, datatxt=None, transform=None, target_transform=None):  # 初始化一些需要传入的参数
        if not datatxt==None:
            super(MyDataset, self).__init__()
            fh = open(root + datatxt, 'r')  # 按照传入的路径和txt文本参数，打开这个文本，并读取内容
            imgs = []   # 创建一个名为img的空列表，一会儿用来装东西
            for line in fh: # 按行循环txt文本中的内容
                line = line.rstrip()    # 删除 本行string 字符串末尾的指定字符，这个方法的详细介绍自己查询python
                words = line.split()  # 通过指定分隔符对字符串进行切片，默认为所有的空字符，包括空格、换行、制表符等
                imgs.append(('./StampDB'+ words[0], int(words[1])))  # 把txt里的内容读入imgs列表保存，具体是words几要看txt内容而定
         # 很显然，根据我刚才截图所示txt的内容，words[0]是图片信息，words[1]是lable
            self.imgs = imgs
            self.transform = transform
            self.target_transform = target_transform
        #单张图片直接读取路径
        elif datatxt == None:
            super(MyDataset, self).__init__()
            imgs = []
            imgs.append((root,int(-1)))
            self.imgs = imgs
            self.transform = transform
            self.target_transform = target_transform


    def __getitem__(self, index):
        # 这个方法是必须要有的，用于按照索引读取每个元素的具体内容
        fn, label = self.imgs[index]  # fn是图片path #fn和label分别获得imgs[index]也即是刚才每行中word[0]和word[1]的信息
        # img = cv2.imread('./StampDB' + fn)
        img = cv2.imread(fn)
        img = cv2.resize(img,(64,64))
        hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
        lower_blue_1 = np.array([0, 25, 2])
        upper_blue_1 = np.array([10, 255, 255])
        lower_blue_2 = np.array([156,25,2])
        upper_blue_2 = np.array([180,255,255])

        mask1 = cv2.inRange(hsv, lower_blue_1, upper_blue_1)
        mask2 = cv2.inRange(hsv, lower_blue_2, upper_blue_2)
        mask = mask1+mask2
        #为啥变蓝色了
        new_image = img * (mask[:, :, None].astype(img.dtype))
        img=new_image

        if self.transform is not None:
            img = self.transform(img)  # 是否进行transform
        return img, label# return很关键，return回哪些内容，那么我们在训练时循环读取每个batch时，就能获得哪些内容

    def __len__(self):  # 这个函数也必须要写，它返回的是数据集的长度，也就是多少张图片，要和loader的长度作区分
        return len(self.imgs)