#pragma once

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"

#include <iostream>
using namespace std;

//打印4*4矩阵，glm矩阵以列为主，打印时转置为行为主
ostream& operator<<(ostream& output, const glm::mat4& m)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			output << m[j][i] << " ";
		output << endl;
	}
	return output;
}
//打印4维向量
ostream& operator<<(ostream& output, const glm::vec4& v)
{
	output << "[";
	for (int i = 0; i < 4; i++)
	{
		output << v[i] << " ";
	}
	output << "]";
	return output;
}
//打印3维向量
ostream& operator<<(ostream& output, const glm::vec3& v)
{
	output << "[";
	for (int i = 0; i < 3; i++)
	{
		output << v[i] << " ";
	}
	output << "]";
	return output;
}
//交换x和y的值
void swap(int& x, int& y)
{
	int z = x;
	x = y;
	y = z;
}
typedef struct { unsigned char r, g, b; } Color; //r，g，b取值范围为0-255
//帧缓存和深度缓存，记录图像宽度和高度信息
typedef struct { unsigned char* color; float* z;  int width, height; } Buffer;
//处理绘制像素时坐标超出窗口异常的类
class XY_ERROR
{
public:
    int x, y;
    XY_ERROR(int x, int y) :x(x), y(y) {}
    void echo() { cout << "screen XY error: [" << x << "," << y << "]"; }
};
//设置像素[x,y]的颜色，默认使用前景色
void setPixel(Buffer frame_buffer, int x, int y, Color c)
{
    //坐标超出窗口坐标范围时抛出异常
    if (x < 0 || x >= frame_buffer.width)
    {
        throw(XY_ERROR(x, y));
        return;
    }
    if (y < 0 || y >= frame_buffer.height)
    {
        throw(XY_ERROR(x, y));
        return;
    }

    //idx表示像素[x,y]的颜色在color数组中的存储位置
    int idx = y * frame_buffer.width + x;
    frame_buffer.color[idx * 3] = c.r;
    frame_buffer.color[idx * 3 + 1] = c.g;
    frame_buffer.color[idx * 3 + 2] = c.b;
}

//获取像素[x,y]的颜色，返回Color对象
Color getPixel(Buffer frame_buffer, int x, int y)
{
    //idx表示像素[x,y]的颜色在color数组中的存储位置
    int idx = y * frame_buffer.width + x;
    return { frame_buffer.color[idx * 3], frame_buffer.color[idx * 3 + 1], frame_buffer.color[idx * 3 + 2] };
}

//设置像素[x,y]的深度
void setDepth(Buffer z_buffer, int x, int y, float d)
{
    //idx表示像素[x,y]的深度在z数组中的存储位置
    int idx = y * z_buffer.width + x;
    z_buffer.z[idx] = d;
}

//读取像素[x,y]的深度
float getDepth(Buffer z_buffer, int x, int y)
{
    //idx表示像素[x,y]的深度在z数组中的存储位置
    int idx = y * z_buffer.width + x;
    return z_buffer.z[idx];
}

//将颜色缓存设置为背景色，深度缓存设置为最大值
void clearScreen(Buffer buffer, Color background_color)
{
    for (unsigned x = 0; x < buffer.width; x++)
    {
        for (unsigned y = 0; y < buffer.height; y++)
        {
            setPixel(buffer, x, y, background_color);
            setDepth(buffer, x, y, FLT_MAX);
        }
    }
}

//中点画线算法的实现，绘制结果保存到frame_buffer的color数组中
void MidPointLine(Buffer frame_buffer, int x0, int y0, int x1, int y1,
    Color c)
{
    //处理斜率绝对值大于1的情况
    bool steep = abs(int(y1 - y0)) > abs(int(x1 - x0));
    if (steep == true)
    {
        swap(x0, y0);
        swap(x1, y1);
    }

    //起点x坐标大于终点x坐标
    if (x0 > x1)
    {
        swap(x0, x1);
        swap(y0, y1);
    }

    //下降直线
    bool descend = y0 > y1;
    int offset = 2 * y0;
    if (descend == true)
    {
        y1 = offset - y1;
    }

    //初始化循环变量
    int dx = x1 - x0;
    int dy = y1 - y0;
    int dm = 2 * dy - dx;
    int dmp1 = 2 * dy;
    int dmp2 = 2 * (dy - dx);

    //循环遍历每一列，根据中点判别式符号选择离直线最近的点
    for (int x = x0, y = y0; x <= x1; x++)
    {
        int real_y = y;
        if (descend == true)
            real_y = offset - y;

        if (steep == true)
            setPixel(frame_buffer, real_y, x, c);
        else
            setPixel(frame_buffer, x, real_y, c);

        if (dm <= 0)    //中点在直线上方
            dm = dm + dmp1;
        else
        {
            dm = dm + dmp2;
            y++;
        }
    }
}