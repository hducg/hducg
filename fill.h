#pragma once

#include "util.h"

#include <vector>
#include <fstream>

//��������������Ĳ�ֵ�ľ���ֵС����ֵ������Ϊ�������
const float THRESHOLD = 0.000001;

//�߱�ͻ�߱����õ��ı߽ڵ�ṹ
class edge
{
public:
    int y_max;              //�ߵ����yֵ
    float x;                //���뵱ǰɨ���߽����x����
    float dx;               //ɨ�����������1ʱ��x������
    edge* next = nullptr;   //�뵱ǰɨ�����ཻ�ı��γ�һ����������x��С��������
};

//����<��������ж��뵱ǰɨ�����ཻ�������ߣ�e1�Ƿ���e2֮ǰ
//�漰�����������㣬����������THERESHOLD��������С
bool operator<(const edge& e1, const edge& e2)
{
    //�����ж�x�Ĵ�С�����e1.x < e2.x����e1��ǰ
    if (e2.x - e1.x > THRESHOLD)
        return true;
    //����e1�ں�
    else if (e1.x - e2.x > THRESHOLD)
        return false;
    //������ߵ�xֵ��ȣ����ж�e1.dx<e2.dx�Ƿ����
    else
        return e2.dx - e1.dx > THRESHOLD;
}

//�ڱ�start֮������e
//start��ʾ���������ĵ�һ���ߣ�����e֮��������ά������״̬
void insertEdge(edge*& start, edge* e)
{
    //start����Ϊ�գ���Ӧ�ڱ߱���߱�ĳ�ʼ״̬����ʱ��e��Ϊ�µ�start
    if (start == nullptr)
    {
        start = e;
        return;
    }

    //���e��˳����start֮ǰ��Ҳ��Ҫ��start�޸�Ϊe
    if (*e < *start)
    {
        e->next = start;
        start = e;
        return;
    }

    //������������������ҵ�һ�������prev    
    edge* prev = start;
    edge* next = prev->next;
    while (next != nullptr)
    {
        //prev��˳����e֮ǰ����next��˳����e֮��
        if (*prev < *e && * e < *next)
        {
            break;
        }
        prev = next;
        next = next->next;
    }
    //�ҵ��������޸����ӹ�ϵ
    prev->next = e;
    e->next = next;
}

//��߱�ı�����ʼ��Ӧ��Ϊż��������������쳣
//�����ʾ��߱��쳣����
class AEL_ERROR
{
public:
    void echo() { cout << "active edge list error"; }
};

//��ʾ��������ݽṹ���������������
typedef struct { float x, y, z; } Vertex;

//����+�������������Vertex�ĺͣ�����һ��Vertex����
Vertex operator+(const Vertex& v1, const Vertex& v2)
{
    return Vertex{ v1.x + v2.x,v1.y + v2.y,v1.z + v2.z };
}

//�����򶥵������ʾ����εĻ�
typedef vector<Vertex> Loop;

//�������
class Polygon
{
public:
    //һ������ο����ж���������������У���һ������ʾ�⻷�����໷Ϊ�ڻ�
    vector<Loop> loops; 
    //���ɫ
    Color color;
    //�Ƿ�������Ȳ���
    bool depth_enabled;
    //��ɫ����Ȼ������飬���ڱ�����Ƶ�ͼ������
    Buffer buffer;
    //bottom��top�ֱ��ʾ��������α任����Ļ����ϵ����ײ��������������y����ֵ
    int bottom, top;
    
    //���Ʊ߽�
    void draw(Color c);
    //ɨ�������������
    void fill(bool depth_enabled = false);
    //ɨ����������������õ���������
    float interpolateDepth(int x, int y);           //��Ȳ�ֵ
    void fillInterval(float x1, float x2, int y);   //���ɨ����y�ϵ�����[x1,x2]
    void getScanRange();                            //��������y����ķ�Χ����bottom��top��ֵ
    void setupEdgeTable(vector<edge*>& edge_table); //�����߱�
    
    //������ر��������
    void setDepthTest(bool depth_enabled) { this->depth_enabled = depth_enabled; }

    //���ߺ���
    void save();    //��������Ķ���α��浽�ļ��У�����debug
    void load();    //���ļ��ж�ȡsave�Ķ���Σ�����debug

    //���캯��
    Polygon(vector<Loop> loops, Color c, Buffer b) 
        :loops(loops), color(c), buffer(b), depth_enabled(false)
    {
    }
    Polygon(Buffer b) :buffer(b), depth_enabled(false), color({255,255,255})
    {
    }
};
//������������������ӡ���������
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
//������������������ӡ����������
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
    //����ÿ����
    for (int i = 0; i < loops.size(); i++)
    {
        //��������ÿ������
        for (int j = 0; j < loops[i].size(); j++)
        {
            //k�ǵ�j��������¸����㣬���һ��������¸������ǵ�һ������
            int k = (j + 1) % loops[i].size();
            //�Ը�����������������룬ת��Ϊ��������
            int x0 = loops[i][j].x + 0.5;
            int y0 = loops[i][j].y + 0.5;
            int x1 = loops[i][k].x + 0.5;
            int y1 = loops[i][k].y + 0.5;
            //�������Ӷ���j��k�ı�
            MidPointLine(buffer, x0, y0, x1, y1, c);
        }
    }
}

void Polygon::fill(bool depth_enabled)
{
    setDepthTest(depth_enabled);
    //1. ��ʼ���߱�ͻ�߱�
    vector<edge*> edge_table;           //�߱�
    getScanRange();                     //��������y����ķ�Χ
    setupEdgeTable(edge_table);         //�����߱�
    edge* active_edge_list = nullptr;   //��ʼ����߱�active_edge_listָ���뵱ǰɨ�����ཻ�ĵ�һ����
    int y = bottom;                     //ɨ���ߵĳ�ֵΪ������¶˵��y����
    //2. ������bottom��top��ɨ����
    try
    {
        do
        {
            //��ɨ����y�ཻ�ıߴ洢�ڱ߱�ĵ�y-bottom��Ԫ��
            if (edge_table[y])     //�������εı���ɨ����y�ཻ�����Ӧ�߱�Ԫ�ز�Ϊ��
            {
                //2-1 ���߱�����ɨ����y�ཻ�����б߼����߱�
                edge* tmpEdge = edge_table[y];
                while (tmpEdge != nullptr)
                {
                    //���޸�tmpEdge��nextָ��֮ǰ���ȱ���ɵ�next
                    edge* next = tmpEdge->next;
                    tmpEdge->next = nullptr;
                    //��tmpEdge����active_edge_list
                    insertEdge(active_edge_list, tmpEdge);
                    //����߱��е���һ����
                    tmpEdge = next;
                }
            }
            //2-2 �Ի�߱��еı߽�����Ժ����
            edge* e1 = active_edge_list;    //�󽻵�
            while (e1)
            {
                edge* e2 = e1->next;        //�ҽ���
                if (e2 == nullptr)          //û�����󽻵���ԵĽ��㣬�׳��쳣
                {
                    throw(AEL_ERROR());
                }
                //����󽻵���ҽ���֮�������
                fillInterval(e1->x, e2->x, y);
                e1 = e2->next;  //��һ��������󽻵�
            }
            //2-3 ɾ���ѵ�ͷ�ı�
            e1 = active_edge_list;  //�ӵ�һ���߿�ʼ
            edge* prev = nullptr;   //��¼��ɾ���ڵ��ǰһ���ڵ�
            while (e1)
            {
                if (e1->y_max == y) //�ѵ����e1���϶��㣬ɾ���ı�
                {
                    if (prev)   //���ǻ�߱�ĵ�һ����
                    {
                        //�޸����ӹ�ϵ
                        prev->next = e1->next;
                        delete e1;
                        //������һ����
                        e1 = prev->next;
                    }
                    else        //�ǻ�߱�ĵ�һ���ߣ�ɾ����Ӧ����active_edge_listָ��
                    {
                        active_edge_list = e1->next;
                        delete e1;
                        e1 = active_edge_list;
                    }
                }
                //������ûɨ����ı�
                else
                {
                    prev = e1;
                    e1 = e1->next;
                }
            }
            //2-4 ���½�������
            e1 = active_edge_list;
            while (e1)
            {
                e1->x += e1->dx;
                e1 = e1->next;
            }
            y++;
            //ɨ�������������ʱ��ѭ������
        } while (y <= top || active_edge_list != nullptr);
    }
    //�����߱��쳣������debug
    catch (AEL_ERROR ael)
    {
        ael.echo();
        cout << endl;
        cout << *this;              //��ӡ�����
        cout << "y=" << y << endl;  //��ӡɨ��������
        cout << active_edge_list;   //��ӡ��߱�
        //save();                   //��������Ķ���α��浽�ļ�������debug
    }
}

void Polygon::getScanRange()
{
    //�ҵ����ж���������y�����ֵ����Сֵ
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

    //ת��Ϊ��������
    bottom = y_min + 0.5;
    top = y_max + 0.5;
}

void Polygon::setupEdgeTable(vector<edge*>& edge_table)
{
    edge_table.resize(buffer.height);   //�����СΪͼ��߶�
    //����ÿ����
    for (int i = 0; i < loops.size(); i++)
    {
        //��������ÿ������
        for (int j = 0; j < loops[i].size(); j++)
        {
            //��lower��¼�Ե�j������Ϊ���ı�yֵ��С���Ǹ������������upper��¼yֵ�ϴ�Ķ��������
            int lower = j, upper = (j + 1) % loops[i].size();

            //����ߵ��������������y������ȣ���Ϊˮƽ�ߣ�������߱�
            int y_lower = loops[i][lower].y + 0.5, y_upper = loops[i][upper].y + 0.5;
            if (y_lower == y_upper)
                continue;

            //����y����ϴ󣬽���lower��upper
            if (y_lower > y_upper)
            {
                swap(upper, lower);
                y_lower = y_upper;
            }

            //k��ʾ����lower���ڵ���һ�����ϵ���һ�����������
            //k������lower֮ǰ(���lower�Ǳߵ����)��Ҳ������lower֮��(���lower�Ǳߵ��յ�)
            int k = lower + lower - upper;
            if (k < 0) k += loops[i].size();
            if (k >= loops[i].size()) k -= loops[i].size();

            //�½��߽ڵ�
            edge* newEdge = new edge;
            //y_maxΪ�϶����y���꣬���������תΪ��������
            newEdge->y_max = loops[i][upper].y + 0.5;
            newEdge->x = loops[i][lower].x; //�¶���Ϊ��һ������
            newEdge->dx = (loops[i][upper].x - loops[i][lower].x) /
                (loops[i][upper].y - loops[i][lower].y);

            //���ڱߵ���һ�������y����
            int y = loops[i][k].y + 0.5;
            //lower����Ϊ�Ǽ�ֵ�㣬���Ͻض�һ������            
            if (y_lower >= y)
            {
                y_lower += 1;
                newEdge->x += newEdge->dx;
            }
            //TODO: ����ڱ���ˮƽ�ߣ�
            else if (y_lower == y)
            {

            }
            //���߲���߱��е�y_lower����Ԫ��������
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
                //����ö������Ͷ���ڸ���Ļ����Ķ�Ӧ������
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
    //˫���Բ�ֵ���㵱ǰ��zֵ
    //�ֱ��¼���ҽ���
    glm::vec3 crossPoint[2] = { {0,0,0} };
    glm::vec3 topPoint[4] = { {0,0,0} };
    float x_distance[2] = { buffer.width };
    for (int i = 0; i < loops.size(); i++)
    {
        for (int j = 0; j < loops[i].size(); j++)
        {
            //��ǰ����y��ֱ���ҵ��뵱ǰ����α߾�����̵����������Լ��ֱཻ�ߵ��ĸ�����
            //����������
            float t2;
            //t2Ϊ��ǰ��������һ�����㹹�ɵ�ֱ���������ĺ��߽���Ĳ���
            t2 = (100 * (loops[i][j].y - y)) / (-(loops[i][(j + 1) % loops[i].size()].y - loops[i][j].y) * 100);
            float xt;
            xt = loops[i][j].x + t2 * (loops[i][(j + 1) % loops[i].size()].x - loops[i][j].x);
            if (t2 >= 0 && t2 <= 1)
            {
                //�н���
                if (xt - x >= 0)
                {
                    //�ҽ���
                    if (xt - x < x_distance[1])
                    {
                        //�ж��Ƿ�Ϊ�������
                        //����ȵ�ǰ�ҽ�������滻
                        x_distance[1] = xt - x;
                        crossPoint[1].x = xt;
                        crossPoint[1].y = y;
                        //���Բ�ֵ��z
                        float z1 = loops[i][(j + 1) % loops[i].size()].z;
                        float z2 = loops[i][j].z;

                        crossPoint[1].z = (1 / z1 - 1 / z2) * t2 + 1 / z2;
                    }
                }
                else
                {
                    //�󽻵�
                    if (x - xt < x_distance[0])
                    {
                        //�ж��Ƿ�Ϊ�������
                        //����ȵ�ǰ�ҽ�������滻
                        x_distance[0] = x - xt;
                        crossPoint[0].x = xt;
                        crossPoint[0].y = y;
                        //���Բ�ֵ��z
                        float z1 = loops[i][(j + 1) % loops[i].size()].z;
                        float z2 = loops[i][j].z;
                        crossPoint[0].z = (1 / z1 - 1 / z2) * t2 + 1 / z2;
                    }
                }

            }
        }
    }
    //����������ֵ������zֵ
    float z = (crossPoint[1].z - crossPoint[0].z) * (x - crossPoint[0].x) / (crossPoint[1].x - crossPoint[0].x) + crossPoint[0].z;
    return 1 / z;
    //return 0.0f;
}

void Polygon::save()
{
    //�ö������ļ�������������
    ofstream fout("polygon", ios::binary);
    //��¼��������
    int num_loops = loops.size();
    fout.write(reinterpret_cast<char*>(&num_loops), sizeof(int));
    for (int i = 0; i < loops.size(); i++)
    {
        //��¼ÿ�����Ķ�������
        int num_vertices = loops[i].size();
        fout.write(reinterpret_cast<char*>(&num_vertices), sizeof(int));
        //��¼�������ж�������
        fout.write(reinterpret_cast<char*>(loops[i].data()), sizeof(Vertex) * num_vertices);
    }
    fout.close();
}

void Polygon::load()
{
    ifstream fin("../polygon", ios::binary);
    //��ȡ��������
    int num_loops;
    fin.read(reinterpret_cast<char*>(&num_loops), sizeof(int));
    for (int i = 0; i < num_loops; i++)
    {
        //��ȡÿ�����Ķ�������
        int num_vertices;
        fin.read(reinterpret_cast<char*>(&num_vertices), sizeof(int));
        //��Loop���涥������
        Loop loop;
        loop.resize(num_vertices);  //���������С
        //�����������ȡ��vector���������
        fin.read(reinterpret_cast<char*>(loop.data()), sizeof(Vertex) * num_vertices);
        //�����������εĻ�����
        loops.push_back(loop);
    }
    fin.close();
}