#服务器上的torch版本有点老，需要修改一些代码
import torch
from torchvision import transforms
from torchvision import models
import torch.utils.data as Data
import torch.nn as nn
import torch.optim as optim
import numpy as np

#myfunction
import DataOperation

use_cuda = torch.cuda.is_available()
# Hyper Parameters
EPOCH = 50              # train the training data n times, to save time, we just train 1 epoch
BATCH_SIZE = 50
LR = 0.001              # learning rate

# 根据自己定义的那个MyDataset来创建数据集！注意是数据集！而不是loader迭代器
train_data = DataOperation.MyDataset('./StampDB/', 'train.txt', transform=transforms.ToTensor())
test_data = DataOperation.MyDataset('./StampDB/', 'test.txt', transform=transforms.ToTensor())
# valid_data = DataOperation.MyDataset('./StampDB/', 'valid.txt', transform=transforms.ToTensor())


train_loader = Data.DataLoader(dataset=train_data, batch_size=BATCH_SIZE, shuffle=True)
test_loader = Data.DataLoader(dataset=test_data, batch_size=BATCH_SIZE)
# valid_loader = Data.DataLoader(dataset=valid_data,batch_size=BATCH_SIZE)

model=models.resnet50(pretrained=False)
#if torch 0.4.2
adp = torch.nn.AdaptiveAvgPool2d(list(np.array([1,1])))
model.avgpool=adp

print(model)  # net architecture

loss_func = nn.CrossEntropyLoss()                       # the target label is not one-hotted
# optimizer = torch.optim.Adam(model.parameters(), lr=LR)   # optimize all cnn parameters
optimizer = optim.SGD(model.parameters(), lr=LR, momentum=0.9, weight_decay=5e-4) #优化方式为mini-batch momentum-SGD，并采用L2正则化（权重衰减）

# training and testing
for epoch in range(EPOCH):
    model.train()
    #b_x数据，b_y标签
    for step, (b_x, b_y) in enumerate(train_loader):   # gives batch data, normalize x when iterate train_loader
        output = model(b_x)
        loss = loss_func(output, b_y)   # cross entropy loss
        optimizer.zero_grad()           # clear gradients for this training step
        loss.backward()                 # backpropagation, compute gradients
        optimizer.step()                # apply gradients

    model.eval()
    all=0.0
    right=0.0
    for batch_index, (test_x, test_y) in enumerate(test_loader):
        test_output = model(test_x)
        pred_y = torch.max(test_output, 1)[1].data.numpy()
        accuracy = float((pred_y == test_y.data.numpy()).astype(int).sum())
        right = right + accuracy
        all = all + test_y.size(0)
    accuracy=right/all
    print('Epoch: ', epoch, '| train loss: %.4f' % loss.data.numpy(), '| test accuracy: %.2f' % accuracy)

model.eval()
# torch.save(model.state_dict(),'resnet.pkl')
torch.save(model,'resnet.pkl')

#test
"""
BATCH_SIZE = 1
my_data = MyDataset('./StampDB/', 'test.txt', transform=transforms.ToTensor())
my_loader = Data.DataLoader(dataset=my_data, batch_size=BATCH_SIZE)
all=0.0
right=0.0
for batch_index, (test_x, test_y) in enumerate(my_loader):
    test_output = model(test_x)
    pred_y = torch.max(test_output, 1)[1].data.numpy()
    accuracy = float((pred_y == test_y.data.numpy()).astype(int).sum())
    right = right+accuracy
    all = all+test_y.size(0)
    print('index%d'%(batch_index+1), '| predict: ', pred_y, '| real: ', test_y.data.numpy(),'| 是否判断正确 %.2f。'%(accuracy/BATCH_SIZE))
print('| 总体正确率 %.2f。'%(right/all))
"""