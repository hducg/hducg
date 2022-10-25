#pragma once

#include "util.h"

#include <vector>
#include <fstream>

//如果两个浮点数的差值的绝对值小于阈值，则认为它们相等
const float THRESHOLD = 0.000001;

//边表和活动边表中用到的边节点结构
class edge
{
public:
    int y_max;              //边的最大y值
    float x;                //边与当前扫描线交点的x坐标
    float dx;               //扫描线坐标递增1时，x的增量
    edge* next = nullptr;   //与当前扫描线相交的边形成一个链表，按照x从小到大排序
};

//重载<运算符，判断与当前扫描线相交的两条边，e1是否在e2之前
//涉及到浮点数运算，允许有误差，用THERESHOLD控制误差大小
bool operator<(const edge& e1, const edge& e2)
{
    //首先判断x的大小，如果e1.x < e2.x，则e1在前
    if (e2.x - e1.x > THRESHOLD)
        return true;
    //否则，e1在后
    else if (e1.x - e2.x > THRESHOLD)
        return false;
    //如果两边的x值相等，则判断e1.dx<e2.dx是否成立
    else
        return e2.dx - e1.dx > THRESHOLD;
}

//在边start之后插入边e
//start表示有序边链表的第一条边，插入e之后，链表仍维持有序状态
void insertEdge(edge*& start, edge* e)
{
    //start可以为空，对应于边表或活动边表的初始状态，此时将e作为新的start
    if (start == nullptr)
    {
        start = e;
        return;
    }

    //如果e的顺序在start之前，也需要将start修改为e
    if (*e < *start)
    {
        e->next = start;
        start = e;
        return;
    }

    //其余情况，遍历链表，找到一个插入点prev    
    edge* prev = start;
    edge* next = prev->next;
    while (next != nullptr)
    {
        //prev的顺序在e之前，且next的顺序在e之后
        if (*prev < *e && * e < *next)
        {
            break;
        }
        prev = next;
        next = next->next;
    }
    //找到插入点后修改连接关系
    prev->next = e;
    e->next = next;
}

//活动边表的边数量始终应该为偶数个，否则存在异常
//定义表示活动边表异常的类
class AEL_ERROR
{
public:
    void echo() { cout << "active edge list error"; }
};

//表示顶点的数据结构，由三个坐标组成
typedef struct { float x, y, z; } Vertex;

//重载+运算符计算两个Vertex的和，返回一个Vertex对象
Vertex operator+(const Vertex& v1, const Vertex& v2)
{
    return Vertex{ v1.x + v2.x,v1.y + v2.y,v1.z + v2.z };
}

//用有序顶点数组表示多边形的环
typedef vector<Vertex> Loop;

//多边形类
class Polygon
{
public:
    //一个多边形可以有多个环，放在数组中，第一个环表示外环，其余环为内环
    vector<Loop> loops; 
    //填充色
    Color color;
    //是否启用深度测试
    bool depth_enabled;
    //颜色和深度缓存数组，用于保存绘制的图像和深度
    Buffer buffer;
    //bottom和top分别表示，将多边形变换到屏幕坐标系后，最底部顶点和最顶部顶点的y坐标值
    int bottom, top;
    
    //绘制边界
    void draw(Color c);
    //扫描线填充主函数
    void fill(bool depth_enabled = false);
    //扫描线填充主函数调用的其它函数
    float interpolateDepth(int x, int y);           //深度插值
    void fillInterval(float x1, float x2, int y);   //填充扫描线y上的区间[x1,x2]
    void getScanRange();                            //计算多边形y坐标的范围，即bottom和top的值
    void setupEdgeTable(vector<edge*>& edge_table); //建立边表
    
    //开启或关闭深度消隐
    void setDepthTest(bool depth_enabled) { this->depth_enabled = depth_enabled; }

    //工具函数
    void save();    //将有问题的多边形保存到文件中，用于debug
    void load();    //从文件中读取save的多边形，用于debug

    //构造函数
    Polygon(vector<Loop> loops, Color c, Buffer b) 
        :loops(loops), color(c), buffer(b), depth_enabled(false)
    {
    }
    Polygon(Buffer b) :buffer(b), depth_enabled(false), color({255,255,255})
    {
    }
};
//重载输出流运算符，打印多边形数据
ostream& operator<<(ostream& os, const Polygon& polygon)
{
    int num_loops = polygon.loops.size();
    for (int i = 0; i < num_loops; i++)
    {
        os << "loop " << i << ": ";
        int num_vertices = polygon.loops[i].size();
        for (int j = 0; j < num_vertices; j++)
        {
            os << "[" << int(polygon.loops[i][j].x + 0.5) << "," << int(polygon.loops[i][j].y + 0.5) << "]";
            if (j < num_vertices - 1)
                os << "-->";
        }
        os << endl;
    }
    return os;
}
//重载输出流运算符，打印边链表数据
ostream& operator<<(ostream& os, edge* e)
{
    while (e)
    {
        os << "[x: " << e->x << " y_max: " << e->y_max << "]";
        if (e->next)
            os << " --> ";
        e = e->next;
    }
    os << endl;
    return os;
}
void Polygon::draw(Color c)
{
    //遍历每个环
    for (int i = 0; i < loops.size(); i++)
    {
        //遍历环的每个顶点
        for (int j = 0; j < loops[i].size(); j++)
        {
            //k是第j个顶点的下个顶点，最后一个顶点的下个顶点是第一个顶点
            int k = (j + 1) % loops[i].size();
            //对浮点坐标进行四舍五入，转换为像素坐标
            int x0 = loops[i][j].x + 0.5;
            int y0 = loops[i][j].y + 0.5;
            int x1 = loops[i][k].x + 0.5;
            int y1 = loops[i][k].y + 0.5;
            //绘制连接顶点j和k的边
            MidPointLine(buffer, x0, y0, x1, y1, c);
        }
    }
}

void Polygon::fill(bool depth_enabled)
{
    setDepthTest(depth_enabled);
    //1. 初始化边表和活化边表
    vector<edge*> edge_table;           //边表
    getScanRange();                     //计算多边形y坐标的范围
    setupEdgeTable(edge_table);         //建立边表
    edge* active_edge_list = nullptr;   //初始化活化边表，active_edge_list指向与当前扫描线相交的第一条边
    int y = bottom;                     //扫描线的初值为多边形下端点的y坐标
    //2. 遍历从bottom到top的扫描线
    try
    {
        do
        {
            //与扫描线y相交的边存储在边表的第y-bottom个元素
            if (edge_table[y])     //如果多边形的边与扫描线y相交，则对应边表元素不为空
            {
                //2-1 将边表中与扫描线y相交的所有边加入活化边表
                edge* tmpEdge = edge_table[y];
                while (tmpEdge != nullptr)
                {
                    //在修改tmpEdge的next指针之前，先保存旧的next
                    edge* next = tmpEdge->next;
                    tmpEdge->next = nullptr;
                    //将tmpEdge插入active_edge_list
                    insertEdge(active_edge_list, tmpEdge);
                    //处理边表中的下一条边
                    tmpEdge = next;
                }
            }
            //2-2 对活化边表中的边进行配对和填充
            edge* e1 = active_edge_list;    //左交点
            while (e1)
            {
                edge* e2 = e1->next;        //右交点
                if (e2 == nullptr)          //没有与左交点配对的交点，抛出异常
                {
                    throw(AEL_ERROR());
                }
                //填充左交点和右交点之间的区间
                fillInterval(e1->x, e2->x, y);
                e1 = e2->next;  //下一个区间的左交点
            }
            //2-3 删除已到头的边
            e1 = active_edge_list;  //从第一条边开始
            edge* prev = nullptr;   //记录待删除节点的前一个节点
            while (e1)
            {
                if (e1->y_max == y) //已到达边e1的上顶点，删除改边
                {
                    if (prev)   //不是活化边表的第一条边
                    {
                        //修改连接关系
                        prev->next = e1->next;
                        delete e1;
                        //处理下一条边
                        e1 = prev->next;
                    }
                    else        //是活化边表的第一条边，删除后应更新active_edge_list指针
                    {
                        active_edge_list = e1->next;
                        delete e1;
                        e1 = active_edge_list;
                    }
                }
                //保留还没扫描完的边
                else
                {
                    prev = e1;
                    e1 = e1->next;
                }
            }
            //2-4 更新交点坐标
            e1 = active_edge_list;
            while (e1)
            {
                e1->x += e1->dx;
                e1 = e1->next;
            }
            y++;
            //扫描完整个多边形时，循环结束
        } while (y <= top || active_edge_list != nullptr);
    }
    //处理活化边表异常，用于debug
    catch (AEL_ERROR ael)
    {
        ael.echo();
        cout << endl;
        cout << *this;              //打印多边形
        cout << "y=" << y << endl;  //打印扫描线坐标
        cout << active_edge_list;   //打印活化边表
        //save();                   //将有问题的多边形保存到文件，单独debug
    }
}

void Polygon::getScanRange()
{
    //找到所有顶点坐标中y的最大值和最小值
    float y_max = loops[0][0].y;
    float y_min = loops[0][0].y;
    for (int i = 0; i < loops.size(); i++)
    {
        for (int j = 0; j < loops[i].size(); j++)
        {
            if (loops[i][j].y - y_max > THRESHOLD)
                y_max = loops[i][j].y;
            if (y_min - loops[i][j].y > THRESHOLD)
                y_min = loops[i][j].y;
        }

    }

    //转换为像素坐标
    bottom = y_min + 0.5;
    top = y_max + 0.5;
}

void Polygon::setupEdgeTable(vector<edge*>& edge_table)
{
    edge_table.resize(buffer.height);   //数组大小为图像高度
    //遍历每个环
    for (int i = 0; i < loops.size(); i++)
    {
        //遍历环的每个顶点
        for (int j = 0; j < loops[i].size(); j++)
        {
            //用lower记录以第j个顶点为起点的边y值较小的那个顶点的索引，upper记录y值较大的顶点的索引
            int lower = j, upper = (j + 1) % loops[i].size();

            //如果边的两个顶点的像素y坐标相等，则为水平边，不加入边表
            int y_lower = loops[i][lower].y + 0.5, y_upper = loops[i][upper].y + 0.5;
            if (y_lower == y_upper)
                continue;

            //起点的y坐标较大，交换lower和upper
            if (y_lower > y_upper)
            {
                swap(upper, lower);
                y_lower = y_upper;
            }

            //k表示顶点lower所在的另一条边上的另一个顶点的索引
            //k可能在lower之前(如果lower是边的起点)，也可能在lower之后(如果lower是边的终点)
            int k = lower + lower - upper;
            if (k < 0) k += loops[i].size();
            if (k >= loops[i].size()) k -= loops[i].size();

            //新建边节点
            edge* newEdge = new edge;
            //y_max为上顶点的y坐标，四舍五入后转为像素坐标
            newEdge->y_max = loops[i][upper].y + 0.5;
            newEdge->x = loops[i][lower].x; //下顶点为第一个交点
            newEdge->dx = (loops[i][upper].x - loops[i][lower].x) /
                (loops[i][upper].y - loops[i][lower].y);

            //相邻边的另一个顶点的y坐标
            int y = loops[i][k].y + 0.5;
            //lower顶点为非极值点，向上截断一个像素            
            if (y_lower >= y)
            {
                y_lower += 1;
                newEdge->x += newEdge->dx;
            }
            //TODO: 如果邻边是水平边？
            else if (y_lower == y)
            {

            }
            //将边插入边表中第y_lower个单元处的链表
            insertEdge(edge_table[y_lower], newEdge);
        }
    }
}

void Polygon::fillInterval(float x1, float x2, int y)
{
    try
    {
        for (int x = x1 + 1; x <= x2; x++)
        {
            if (depth_enabled)
            {
                //计算该多边形中投射在该屏幕坐标的对应点的深度
                float d = interpolateDepth(x, y);
                if (getDepth(buffer, x, y) - d > THRESHOLD)
                {
                    setDepth(buffer, x, y, d);
                    setPixel(buffer, x, y, color);
                }
            }
            else
                setPixel(buffer, x, y, color);
        }
    }
    catch (XY_ERROR xy)
    {
        xy.echo();
        cout << endl;
        cout << "x1=" << x1 << ",x2=" << x2 << endl;
    }
}
//TODO: debug
float Polygon::interpolateDepth(int x, int y)
{
    //双线性插值计算当前点z值
    //分别记录左右交点
    glm::vec3 crossPoint[2] = { {0,0,0} };
    glm::vec3 topPoint[4] = { {0,0,0} };
    float x_distance[2] = { buffer.width };
    for (int i = 0; i < loops.size(); i++)
    {
        for (int j = 0; j < loops[i].size(); j++)
        {
            //当前点沿y作直线找到与当前多边形边距离最短的两个交点以及相交直线的四个顶点
            //参数方程求交
            float t2;
            //t2为当前顶点与下一个顶点构成的直线与所求点的横线交点的参数
            t2 = (100 * (loops[i][j].y - y)) / (-(loops[i][(j + 1) % loops[i].size()].y - loops[i][j].y) * 100);
            float xt;
            xt = loops[i][j].x + t2 * (loops[i][(j + 1) % loops[i].size()].x - loops[i][j].x);
            if (t2 >= 0 && t2 <= 1)
            {
                //有交点
                if (xt - x >= 0)
                {
                    //右交点
                    if (xt - x < x_distance[1])
                    {
                        //判断是否为最近交点
                        //如果比当前右交点近则替换
                        x_distance[1] = xt - x;
                        crossPoint[1].x = xt;
                        crossPoint[1].y = y;
                        //线性插值求z
                        float z1 = loops[i][(j + 1) % loops[i].size()].z;
                        float z2 = loops[i][j].z;

                        crossPoint[1].z = (1 / z1 - 1 / z2) * t2 + 1 / z2;
                    }
                }
                else
                {
                    //左交点
                    if (x - xt < x_distance[0])
                    {
                        //判断是否为最近交点
                        //如果比当前右交点近则替换
                        x_distance[0] = x - xt;
                        crossPoint[0].x = xt;
                        crossPoint[0].y = y;
                        //线性插值求z
                        float z1 = loops[i][(j + 1) % loops[i].size()].z;
                        float z2 = loops[i][j].z;
                        crossPoint[0].z = (1 / z1 - 1 / z2) * t2 + 1 / z2;
                    }
                }

            }
        }
    }
    //在两交点间插值求最终z值
    float z = (crossPoint[1].z - crossPoint[0].z) * (x - crossPoint[0].x) / (crossPoint[1].x - crossPoint[0].x) + crossPoint[0].z;
    return 1 / z;
    //return 0.0f;
}

void Polygon::save()
{
    //用二进制文件保存多边形数据
    ofstream fout("polygon", ios::binary);
    //记录环的数量
    int num_loops = loops.size();
    fout.write(reinterpret_cast<char*>(&num_loops), sizeof(int));
    for (int i = 0; i < loops.size(); i++)
    {
        //记录每个环的顶点数量
        int num_vertices = loops[i].size();
        fout.write(reinterpret_cast<char*>(&num_vertices), sizeof(int));
        //记录环的所有顶点坐标
        fout.write(reinterpret_cast<char*>(loops[i].data()), sizeof(Vertex) * num_vertices);
    }
    fout.close();
}

void Polygon::load()
{
    ifstream fin("../polygon", ios::binary);
    //读取环的数量
    int num_loops;
    fin.read(reinterpret_cast<char*>(&num_loops), sizeof(int));
    for (int i = 0; i < num_loops; i++)
    {
        //读取每个环的顶点数量
        int num_vertices;
        fin.read(reinterpret_cast<char*>(&num_vertices), sizeof(int));
        //用Loop保存顶点坐标
        Loop loop;
        loop.resize(num_vertices);  //设置数组大小
        //将顶点坐标读取到vector对象的数组
        fin.read(reinterpret_cast<char*>(loop.data()), sizeof(Vertex) * num_vertices);
        //将环加入多边形的环数组
        loops.push_back(loop);
    }
    fin.close();
}