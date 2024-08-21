#ifndef DBSCAN_hh
#define DBSCAN_hh 1
#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <iosfwd>
#include <math.h>
#include "DataPoint.hh"
#include "json.hh"

using namespace std;
using json = nlohmann::basic_json<nlohmann::ordered_map>;

// 聚类分析类型
class DBSCAN
{
private:
    vector<DataPoint> dadaSets; // 数据集合
    vector<float> angle;        // 角度
    unsigned int dimNum;        // 维度
    double radius;              // 半径
    unsigned int dataNum;       // 数据数量
    unsigned int minPTs;        // 邻域最小数据个数

    double GetDistance(DataPoint &dp1, DataPoint &dp2);             // 距离函数
    void SetArrivalPoints(DataPoint &dp);                           // 设置数据点的领域点列表
    void KeyPointCluster(unsigned long i, unsigned long clusterId); // 对数据点领域内的点执行聚类操作
public:
    DBSCAN() {}                                           // 默认构造函数
    bool Init(char *fileName, double radius, int minPTs); // 初始化操作
    bool Init_scater(vector<vector<float>> scater, double radius, int minPTs);
    bool DoDBSCANRecursive();         // DBSCAN递归算法
    bool WriteToFile(char *fileName); // 将聚类结果写入文件
    bool WriteToJSON(char *fileName); // 将聚类结果写入JSON文件
};
#endif