import torch
from torchvision import transforms
import time

#myfunction
import DataOperation

t0 = time.time()
model = torch.load('resnet.pkl')
model.eval()
print(model)
BATCH_SIZE = 100
my_data = DataOperation.MyDataset('./StampDB/', 'test.txt', transform=transforms.ToTensor())
my_loader = DataOperation.Data.DataLoader(dataset=my_data, batch_size=BATCH_SIZE)
all=0.0
right=0.0
for batch_index, (test_x, test_y) in enumerate(my_loader):
    test_output = model(test_x)
    pred_y = torch.max(test_output, 1)[1].data.numpy()
    accuracy = float((pred_y == test_y.data.numpy()).astype(int).sum())
    right = right+accuracy
    all = all+test_y.size(0)
    # if not pred_y==test_y.data.numpy():
    #     print("索引%d,预测为%d，实际上为%d"%(batch_index,pred_y,test_y))
    print('index%d'%(batch_index+1), '| predict: ', pred_y, '| real: ', test_y.data.numpy(),'| 是否判断正确 %.2f。'%(accuracy/BATCH_SIZE))
print('| 总体正确率 %.2f。'%(right/all))

print (time.time() - t0)