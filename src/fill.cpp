#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "../util.h"    //"../"表示util.h位于本文件的上级目录

#include <iostream>
#include <vector>
using namespace std;

typedef struct { unsigned char r, g, b; } Color;    //颜色 
typedef vector<glm::vec3> Loop;     //顶点环
typedef struct { vector<Loop> loops; Color color; } Polygon;    //一个多边形由多个环构成
typedef vector<Polygon> Object; //物体定义为多边形的集合

int width = 800, height = 800;  //窗口宽度和高度
Color foreground_color = { 255,255,255 };    //前景色
Color background_color = {0,0,0};    //背景色
unsigned char* frame_buffer = new unsigned char[width * height * 3];   //颜色缓存
float* z_buffer = new float[width * height];    //深度缓存

//1. 定义视图变换和投影变换矩阵
glm::vec3 center = glm::vec3(7.5f, 7.5f, 5.0f);
//相机到物体中心的距离
float radius = 20.0f;
//相机的仰角和方位角，控制相机在以center为球心、半径为radius的球面上移动
float elevation = 0.0f, azimuth = 0.0f;
//相机初始位置
glm::vec3 camera = glm::vec3(center.x + radius * cos(elevation) * cos(azimuth),
    center.y + radius * cos(elevation) * sin(azimuth),
    center.z + radius * sin(elevation));
glm::mat4 view = glm::lookAt(camera, center, glm::vec3(0.0f, 0.0f, 1.0f));
//视角为90°，投影窗口尺寸比为1
//近裁剪平面到视点距离为1，远裁剪平面到视点距离为视点到中心点距离+10
glm::mat4 projection = glm::perspective(glm::pi<float>() * 0.5f, 1.0f, 1.0f,
    glm::length(center - camera) + 10.0f);

bool depth_enabled = false;

Object object;  //物体定义为全局变量

//设置像素[x,y]的颜色，默认使用前景色
void setPixel(int x, int y, Color c = foreground_color)
{
    if (x < 0 || x >= width)
    {
        std::cout << "invalid x value: x = " << x << std::endl;
        return;
    }

    if (y < 0 || y >= height)
    {
        std::cout << "invalid y value: y = " << y << std::endl;
        return;
    }

    int idx = y * width + x;
    frame_buffer[idx * 3] = c.r;
    frame_buffer[idx * 3 + 1] = c.g;
    frame_buffer[idx * 3 + 2] = c.b;
}

//获取像素[x,y]的颜色，保存在c数组中
Color getPixel(int x, int y)
{
    int idx = y * width + x;
    return { frame_buffer[idx * 3], frame_buffer[idx * 3 + 1], frame_buffer[idx * 3 + 2] };
}

//设置像素[x,y]的深度
void setDepth(int x, int y, float d)
{
    int idx = y * width + x;
    z_buffer[idx] = d;
}

//读取像素[x,y]的深度
float getDepth(int x, int y)
{
    int idx = y * width + x;
    return z_buffer[idx];
}

//将颜色缓存设置为背景色，深度缓存设置为最大值
void clearScreen()
{
    for (unsigned x = 0; x < width; x++)
    {
        for (unsigned y = 0; y < height; y++)
        {
            setPixel(x,y, background_color);
            setDepth(x, y, FLT_MAX);
        }
    }
}

//交换x和y的值
void swap(int& x, int& y)
{
    int z = x;
    x = y;
    y = z;
}
void MidPointLine(int x0, int y0, int x1, int y1,
    Color c = foreground_color)
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
            setPixel(real_y, x, c);
        else
            setPixel(x, real_y, c);

        if (dm <= 0)    //中点在直线上方
            dm = dm + dmp1;
        else
        {
            dm = dm + dmp2;
            y++;
        }
    }
}

struct edge
{
    int y_max;
    int x;
    float dx;
    edge* next;
};

float interpolateDepth(const Polygon& polygon, int x, int y)
{
    //TODO:用多边形顶点深度值插值计算内部点深度值
    //参考资料：
    //https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/visibility-problem-depth-buffer-depth-interpolation
    return 0.0f;
}
void fillInterval(const Polygon& polygon, int x1, int x2, int y)
{
    for (int x = x1; x <= x2; x++)
    {
        if (depth_enabled)
        {
            float d = interpolateDepth(polygon, x, y);
            if (d < getDepth(x, y))
            {
                setDepth(x, y, d);
                setPixel(x, y, polygon.color);
            }
        }
        else
            setPixel(x, y, polygon.color);
    }
}
void getScanRange(const Polygon& polygon, int& y_min, int & y_max)
{
    //TODO：计算多边形的y坐标范围，保存到y_min和y_max参数中
}

void setupEdgeTable(const Polygon& polygon, vector<edge*>& edge_table)
{
    //TODO:初始化边表
}
void polygonFill(const Polygon& polygon)
{
    int y_min, y_max;
    getScanRange(polygon, y_min, y_max);
    
    //1. 初始化边表和活性边表
    vector<edge*> edge_table(y_max - y_min, nullptr);
    setupEdgeTable(polygon, edge_table);
    edge* active_edge_list = nullptr;
    int y = y_min + 1;
    //2. 遍历边表
    do
    {
        if (edge_table[y - y_min - 1] != nullptr)
        {
            //1.TODO:插入新边
            
        }
        //2.TODO:配对，填充
        //3.TODO:删除已到头的边
        //4.TODO:更新交点坐标
        y++;
    } while (active_edge_list != nullptr);
}
bool isBackFace(const Polygon& polygon)
{
    //TODO:判断面是否为背面
    return false;
}

void renderObject(Object object)
{
    cout << "view matrix" << endl << view << endl;
    cout << "projection matrix" << endl << projection << endl;

    for (int i = 0; i < object.size(); i++)
    {
        Polygon& polygon = object[i];
        if (isBackFace(polygon))
            continue;
        for (int j = 0; j < polygon.loops.size(); j++)
        {
            Loop& loop = polygon.loops[j];
            for (int k = 0; k < loop.size(); k++)
            {
                //参考资料：https://www.songho.ca/opengl/gl_transform.html
                //视图变换和投影变换，obj-->clip
                glm::vec4 pt = projection * view * glm::vec4(loop[k], 1.0f);
                //除以齐次项，clip-->NDC
                loop[k] = glm::vec3(pt) / pt.w;
                //TODO:把NDC坐标转换为屏幕坐标，ndc-->[xw,yw,zw]
            }
        }
        //填充多边形
        polygonFill(polygon);
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    bool update = false;
    if (key == GLFW_KEY_D && action == GLFW_PRESS)
    {
        depth_enabled = !depth_enabled;
        if (depth_enabled)
            cout << "depth test enabled" << endl;
        else
            cout << "depth test disabled" << endl;
        update = true;
    }

    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
    {
        elevation -= glm::radians(10.0f);
        camera = glm::vec3(center.x + radius * cos(elevation) * cos(azimuth),
            center.y + radius * cos(elevation) * sin(azimuth),
            center.z + radius * sin(elevation));
        view = glm::lookAt(camera, center, glm::vec3(0.0f, 0.0f, 1.0f));
        update = true;
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
    {
        elevation += glm::radians(10.0f);
        camera = glm::vec3(center.x + radius * cos(elevation) * cos(azimuth),
            center.y + radius * cos(elevation) * sin(azimuth),
            center.z + radius * sin(elevation));
        view = glm::lookAt(camera, center, glm::vec3(0.0f, 0.0f, 1.0f));
        update = true;
    }
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
    {
        azimuth -= glm::radians(10.0f);
        camera = glm::vec3(center.x + radius * cos(elevation) * cos(azimuth),
            center.y + radius * cos(elevation) * sin(azimuth),
            center.z + radius * sin(elevation));
        view = glm::lookAt(camera, center, glm::vec3(0.0f, 0.0f, 1.0f));
        update = true;
    }
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
    {
        azimuth += glm::radians(10.0f);
        camera = glm::vec3(center.x + radius * cos(elevation) * cos(azimuth),
            center.y + radius * cos(elevation) * sin(azimuth),
            center.z + radius * sin(elevation));
        view = glm::lookAt(camera, center, glm::vec3(0.0f, 0.0f, 1.0f));
        update = true;
    }
    if (update)
    {
        //初始化颜色缓存和深度缓存
        clearScreen();
        //绘制物体
        renderObject(object);
    }
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    glfwSetKeyCallback(window, key_callback);

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        std::cout << "Error: \n", glewGetErrorString(err);
    }
    std::cout << "------------------Help------------------" << endl;
    std::cout << "Press key 'D': enable/disable depth test" << endl;
    std::cout << "Press arrow key: change camera position" << endl;
    
    //2. 定义物体
    //底面8个点
    glm::vec3 pts[8] = { 
        glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 15.0f, 0.0f), 
        glm::vec3(5.0f, 15.0f, 0.0f), glm::vec3(5.0f, 10.0f, 0.0f), 
        glm::vec3(10.0f, 10.0f, 0.0f), glm::vec3(10.0f, 5.0f, 0.0f),
        glm::vec3(15.0f, 5.0f, 0.0f), glm::vec3(15.0f, 0.0f, 0.0f) };
    glm::vec3 h(0.0f, 0.0f, 10.0f); //高度向量
    
    Loop loops[10];
    for (int i = 0; i < 8; i++) //遍历8个底面顶点
    {
        loops[0].push_back(pts[i]); //底面
        loops[1].push_back(pts[7 - i] + h); //顶面
        //侧面
        loops[i + 2].push_back(pts[i]);
        loops[i + 2].push_back(pts[i] + h);
        loops[i + 2].push_back(pts[(i + 1) % 8] + h);
        loops[i + 2].push_back(pts[(i + 1) % 8]);
    }
    //TODO:定义10个面的颜色
    Color colors[10];
    for (int i = 0; i < 10; i++)
    {
        vector<Loop> ply_loops{loops[i]};
        Polygon polygon{ ply_loops, colors[i] };
        object.push_back(polygon);
    }        

    //3. 渲染
    //初始化颜色缓存和深度缓存
    clearScreen();
    //绘制物体
    renderObject(object);
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        
        glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, frame_buffer);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    delete[] frame_buffer;
    delete[] z_buffer;

    glfwTerminate();
    return 0;
}