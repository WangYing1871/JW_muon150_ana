脚本 data_process.sh 用于将6层muon系统的数据分析步骤整合（分包、解码、对应探测器实际位置、寻track、产生json文件）
  主要参数
  -S 分析软件位置
  -A json文件产生位置
  -L link参数，描述使用的板号
  -d 待分析文件名
  -D 是否跳过分包
  -F 是否跳过解包
  -H 是否跳过位置重建
  -T 是否跳过经迹寻找


用例
  bash data_process.sh -d /home/wangying/work/data/muon-7.dat -L 0x3f -D -F -H -T -A /home/wangying/ -S /home/wangying/work/JW_muon150_ana/cpp/

  待分析初始文件： /home/wangying/work/data/muon-7.dat
  使用板号: (0b00111111) 0, 1, 2, 3, 4, 5块
  json文件生成位置： /home/wangying
  分析软件所在位置： /home/wangying/work/JW_muon150_ana/cpp/
  所有步骤都执行
