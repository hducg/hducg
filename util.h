#pragma once

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"

#include <iostream>
using namespace std;

//��ӡ4*4����glm��������Ϊ������ӡʱת��Ϊ��Ϊ��
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
//��ӡ4ά����
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
//��ӡ3ά����
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
//����x��y��ֵ
void swap(int& x, int& y)
{
	int z = x;
	x = y;
	y = z;
}
typedef struct { unsigned char r, g, b; } Color; //r��g��bȡֵ��ΧΪ0-255
//֡�������Ȼ��棬��¼ͼ���Ⱥ͸߶���Ϣ
typedef struct { unsigned char* color; float* z;  int width, height; } Buffer;
//�����������ʱ���곬�������쳣����
class XY_ERROR
{
public:
    int x, y;
    XY_ERROR(int x, int y) :x(x), y(y) {}
    void echo() { cout << "screen XY error: [" << x << "," << y << "]"; }
};
//��������[x,y]����ɫ��Ĭ��ʹ��ǰ��ɫ
void setPixel(Buffer frame_buffer, int x, int y, Color c)
{
    //���곬���������귶Χʱ�׳��쳣
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

    //idx��ʾ����[x,y]����ɫ��color�����еĴ洢λ��
    int idx = y * frame_buffer.width + x;
    frame_buffer.color[idx * 3] = c.r;
    frame_buffer.color[idx * 3 + 1] = c.g;
    frame_buffer.color[idx * 3 + 2] = c.b;
}

//��ȡ����[x,y]����ɫ������Color����
Color getPixel(Buffer frame_buffer, int x, int y)
{
    //idx��ʾ����[x,y]����ɫ��color�����еĴ洢λ��
    int idx = y * frame_buffer.width + x;
    return { frame_buffer.color[idx * 3], frame_buffer.color[idx * 3 + 1], frame_buffer.color[idx * 3 + 2] };
}

//��������[x,y]�����
void setDepth(Buffer z_buffer, int x, int y, float d)
{
    //idx��ʾ����[x,y]�������z�����еĴ洢λ��
    int idx = y * z_buffer.width + x;
    z_buffer.z[idx] = d;
}

//��ȡ����[x,y]�����
float getDepth(Buffer z_buffer, int x, int y)
{
    //idx��ʾ����[x,y]�������z�����еĴ洢λ��
    int idx = y * z_buffer.width + x;
    return z_buffer.z[idx];
}

//����ɫ��������Ϊ����ɫ����Ȼ�������Ϊ���ֵ
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

//�е㻭���㷨��ʵ�֣����ƽ�����浽frame_buffer��color������
void MidPointLine(Buffer frame_buffer, int x0, int y0, int x1, int y1,
    Color c)
{
    //����б�ʾ���ֵ����1�����
    bool steep = abs(int(y1 - y0)) > abs(int(x1 - x0));
    if (steep == true)
    {
        swap(x0, y0);
        swap(x1, y1);
    }

    //���x��������յ�x����
    if (x0 > x1)
    {
        swap(x0, x1);
        swap(y0, y1);
    }

    //�½�ֱ��
    bool descend = y0 > y1;
    int offset = 2 * y0;
    if (descend == true)
    {
        y1 = offset - y1;
    }

    //��ʼ��ѭ������
    int dx = x1 - x0;
    int dy = y1 - y0;
    int dm = 2 * dy - dx;
    int dmp1 = 2 * dy;
    int dmp2 = 2 * (dy - dx);

    //ѭ������ÿһ�У������е��б�ʽ����ѡ����ֱ������ĵ�
    for (int x = x0, y = y0; x <= x1; x++)
    {
        int real_y = y;
        if (descend == true)
            real_y = offset - y;

        if (steep == true)
            setPixel(frame_buffer, real_y, x, c);
        else
            setPixel(frame_buffer, x, real_y, c);

        if (dm <= 0)    //�е���ֱ���Ϸ�
            dm = dm + dmp1;
        else
        {
            dm = dm + dmp2;
            y++;
        }
    }
}