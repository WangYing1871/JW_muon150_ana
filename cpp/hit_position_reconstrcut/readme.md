# 数据组包程序

探测器击中数据组包程序，这个程序是用来解包[fec2det_dec](..\fec2det_dec)的输出数据的，将数据组包成一个个的事例，每个事例中包含了一个或者多个击中的通道的信息。

## 程序参数
+ -f 指定要处理的数据文件名，有多个文件的时候随便指定一个就行
+ -L 指定使用的 link 是哪些。用 16 进制表示，使用了的 link 就将对应的 bit 位置 1；如 0x1 = 0001b 表示第一个 link 被使用，0x2 = 0010b 表示第二个 link 被使用， 0x6 表示第三个和第二个 link 被使用
+ -p 指定使用的位置文件，使用csv格式文件分为四列
  + 第一列是链接编号
  + 第二列是上一个程序解码的idx，用于区分解码的结果
  + 第三列是探测器的层号
  + 第四列标志坐标轴，1表示x轴，0表示y轴
+ -z 指定Z轴坐标文件，一行，以逗号分割，单位为毫米

## 程序运行
### 初始化分析程序
#### 创建输出文件
如下代码创建输出的Tree，其中postion_data是自定义的类，用于存储击中探测器的信息，其中x_nhits和y_nhits是击中的通道数，x和y是击中的通道的坐标，z是探测器的Z轴坐标，x_amp和y_amp是击中的通道的幅度，x_chn_num和y_chn_num是击中的通道的通道号，x_cluster_hole和y_cluster_hole是击中的位置的不联系点数目，trigger_id是触发号，用于区分不同的事例。
```c++
//Some other place
position_data detector_data[cDET_LAYER];
//...
// Create code
hit_ttree = new TTree("fec_hit_data", "FEC HIT");
trigger_id = 0;
hit_ttree->Branch("trigger_id_all", &trigger_id, "trigger_id/I");
for (int i = 0; i < cDET_LAYER; i++)
{
	string dec_name = "dector" + to_string(i);
	hit_ttree->Branch(dec_name.c_str(), &detector_data[i].x_nhits, "x_nhits/I:y_nhits/I:x[25]/D:y[25]/D:z/D:x_amp[25]/D:y_amp[25]/D:x_chn_num[25]/I:y_chn_num[25]/I:x_cluster_hole[25]/I:y_cluster_hole[25]/I");
}
```
#### 读取位置文件

位置文件的使用csv文件，一个示例如下，第一行是给人读的
+ 第一列表示的是link的编号，目前的系统最大用了8个link，可以支持更多，直接填写即可。
+ 第二列是解码的idx编号，在实际使用中，一个FEC板的芯片可能对应了不通的探测器维度，但是解码结果是存在一起的，因此需要一个idx来区分不同的解码结果
  + 在处理的时候为了简单起见，每个link都有4个idx，在后面的layer和is_x中，如果对应的idx没有使用，将layer和is_x设置为-1
  + 因此文件的前两行是给人读的，在程序中我直接读出来，然后丢弃
+ 第三列是探测器的层号，目前的系统最大用了8层，可以支持更多，直接填写即可
+ 第四列是探测器的坐标轴，1表示x轴，0表示y轴

最后读取到的位置信息存到了一个二维数组layer_idx中，定义如下，如果要修改link_num的数量，改这个文件[constant.hh](../../my_pub/common_lib/constant.hh)
```cpp
int layer_idx[cELINK_NUM * 4][2];
```

| link | idx  | layer | is_x |
| ---- | ---- | ----- | ---- |
| 0    | 0    | 1     | 0    |
| 0    | 1    | 0     | 0    |
| 0    | 2    | -1    | -1   |
| 0    | 3    | -1    | -1   |
| 1    | 0    | 3     | 0    |
| 1    | 1    | 2     | 0    |
| 1    | 2    | -1    | -1   |
| 1    | 3    | -1    | -1   |
| 2    | 0    | 1     | 1    |
| 2    | 1    | 0     | 1    |
| 2    | 2    | -1    | -1   |
| 2    | 3    | -1    | -1   |
| 3    | 0    | 3     | 1    |
| 3    | 1    | 2     | 1    |
| 3    | 2    | -1    | -1   |
| 3    | 3    | -1    | -1   |
......

#### 读取Z轴坐标
Z轴坐标只有一行，以逗号分割每个坐标点，读取到的数据存储到z_position中

### 数据读入及处理
这个部分调用fill_data(string data_filename)这个函数来处理，首先是对每个需要处理的Link的root文件进行读入，root文件由[fec2det_dec](../fec2det_dec/)中的程序产生。

在这里的函数中，一次性会初始化cELINK_NUM个TTree，每个TTree对应一个Link

处理的时候首先判断每个Link对应的触发号是否对齐，如果触发号没有对齐，那么找到所有TTree中最大的触发号，然后找到所有link都对齐的触发号。
  + 在[fec2dec_det](../fec2det_dec/)中，每次比较之后，即使对应的触发中这个link没有过阈的信号，也会Fill()一次，保证在解码的过程中不会让触发号不对齐
  + 但是由于文件是10分钟存一次，每个文件之间极大的概率是把USB的包拆分开了的，因此在后续的解包过程中，可能存在某一个link的包被存在了两个文件中，目前的解码程序为了方便，直接定位到数据包的开头，因此这种被分割了的包就会被视为不完整的包，因此可能存在某一个link的触发号一开始和其它 link 的触发号不对齐的情况
  + 实际上在[fec_data_dec](../fec_data_dec/)中考虑了这种情况，在解包的时候会把包尾多余的数据保存下来，下一个文件进来的时候直接拼起来，这样的解包方式会将所有的数据拼到一起，导致数据包比较大。目前还是单独一个文件一个文件的解码，如果对事例有极致追求可以考虑这种模式

### 数据读入之后就进行数据位置判断



