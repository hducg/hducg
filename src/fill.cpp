#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"

#include <iostream>
#include <vector>
using namespace std;

typedef struct { unsigned char r, g, b; } Color;
typedef vector<glm::vec3> Loop;
typedef struct { vector<Loop> loops; Color color; } Polygon;
typedef vector<Polygon> Object;

int width = 800, height = 800;  //���ڿ�Ⱥ͸߶�
Color foreground_color = { 255,255,255 };    //ǰ��ɫ
Color background_color = {0,0,0};    //����ɫ
unsigned char* frame_buffer = new unsigned char[width * height * 3];   //��ɫ����
float* z_buffer = new float[width * height];    //��Ȼ���

//1. ������ͼ�任��ͶӰ�任����
glm::vec3 center = glm::vec3(7.5f, 7.5f, 5.0f);
glm::vec3 camera = glm::vec3(22.5f, 22.5f, 20.0f);
glm::mat4 view = glm::lookAt(camera, center, glm::vec3(0.0f, 0.0f, 1.0f));
//�ӽ�Ϊ90�㣬ͶӰ���ڳߴ��Ϊ1
//���ü�ƽ�浽�ӵ����Ϊ1��Զ�ü�ƽ�浽�ӵ����Ϊ�ӵ㵽���ĵ����+10
glm::mat4 projection = glm::perspective(glm::pi<float>() * 0.5f, 1.0f, 1.0f,
    glm::length(center - camera) + 10.0f);

float elevation = 0.0f, azimuth = 0.0f;
bool depth_enabled = false;

Object object;

//��������[x,y]����ɫ��Ĭ��ʹ��ǰ��ɫ
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

//��ȡ����[x,y]����ɫ��������c������
Color getPixel(int x, int y)
{
    int idx = y * width + x;
    return { frame_buffer[idx * 3], frame_buffer[idx * 3 + 1], frame_buffer[idx * 3 + 2] };
}

//��������[x,y]�����
void setDepth(int x, int y, float d)
{
    int idx = y * width + x;
    z_buffer[idx] = d;
}

//��ȡ����[x,y]�����
float getDepth(int x, int y)
{
    int idx = y * width + x;
    return z_buffer[idx];
}

//����ɫ��������Ϊ����ɫ����Ȼ�������Ϊ���ֵ
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

//����x��y��ֵ
void swap(int& x, int& y)
{
    int z = x;
    x = y;
    y = z;
}
void MidPointLine(int x0, int y0, int x1, int y1,
    Color c = foreground_color)
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
            setPixel(real_y, x, c);
        else
            setPixel(x, real_y, c);

        if (dm <= 0)    //�е���ֱ���Ϸ�
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
    //TODO
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
    //TODO
}

void setupEdgeTable(const Polygon& polygon, vector<edge*>& edge_table)
{
    //TODO
}
void polygonFill(const Polygon& polygon)
{
    int y_min, y_max;
    getScanRange(polygon, y_min, y_max);
    
    //1. ��ʼ���߱�ͻ��Ա߱�
    vector<edge*> edge_table(y_max - y_min, nullptr);
    setupEdgeTable(polygon, edge_table);
    edge* active_edge_list = nullptr;
    int y = y_min + 1;
    //2. �����߱�
    do
    {
        if (edge_table[y - y_min - 1] != nullptr)
        {
            //1.TODO:�����±�
            
        }
        //2.TODO:��ԣ����
        //3.TODO:ɾ���ѵ�ͷ�ı�
        //4.TODO:���½�������
        y++;
    } while (active_edge_list != nullptr);
}

void renderObject(Object object)
{
    for (int i = 0; i < object.size(); i++)
    {
        Polygon& polygon = object[i];
        for (int j = 0; j < polygon.loops.size(); j++)
        {
            Loop& loop = polygon.loops[j];
            for (int k = 0; k < loop.size(); k++)
            {
                //1. �Զ���ζ���ִ����ͼ�任��ͶӰ�任
                glm::vec4 pt = projection * view * glm::vec4(loop[k], 1.0f);
                //��������z��ʾ���
                loop[k] = glm::vec3(pt) / pt.w;
                //TODO:��[x,y]ת��Ϊ��������
            }
        }
        //�������
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
        elevation += glm::radians(10.0f);
        float radius = glm::length(center - camera);
        camera = glm::vec3(center.x + radius * sin(elevation) * cos(azimuth),
            center.y + radius * sin(elevation) * sin(azimuth),
            center.z + radius * cos(elevation));
        view = glm::lookAt(camera, center, glm::vec3(0.0f, 1.0f, 0.0f));
        update = true;
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
    {
        elevation -= glm::radians(10.0f);
        float radius = glm::length(center - camera);
        camera = glm::vec3(center.x + radius * sin(elevation) * cos(azimuth),
            center.y + radius * sin(elevation) * sin(azimuth),
            center.z + radius * cos(elevation));
        view = glm::lookAt(camera, center, glm::vec3(0.0f, 1.0f, 0.0f));
        update = true;
    }
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
    {
        azimuth += glm::radians(10.0f);
        float radius = glm::length(center - camera);
        camera = glm::vec3(center.x + radius * sin(elevation) * cos(azimuth),
            center.y + radius * sin(elevation) * sin(azimuth),
            center.z + radius * cos(elevation));
        view = glm::lookAt(camera, center, glm::vec3(0.0f, 1.0f, 0.0f));
        update = true;
    }
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
    {
        azimuth -= glm::radians(10.0f);
        float radius = glm::length(center - camera);
        camera = glm::vec3(center.x + radius * sin(elevation) * cos(azimuth),
            center.y + radius * sin(elevation) * sin(azimuth),
            center.z + radius * cos(elevation));
        view = glm::lookAt(camera, center, glm::vec3(0.0f, 1.0f, 0.0f));
        update = true;
    }
    if (update)
    {
        //��ʼ����ɫ�������Ȼ���
        clearScreen();
        //��������
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
    
    //2. ��������
    //����8����
    glm::vec3 pts[8] = { 
        glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 15.0f, 0.0f), 
        glm::vec3(5.0f, 15.0f, 0.0f), glm::vec3(5.0f, 10.0f, 0.0f), 
        glm::vec3(10.0f, 10.0f, 0.0f), glm::vec3(10.0f, 5.0f, 0.0f),
        glm::vec3(15.0f, 5.0f, 0.0f), glm::vec3(15.0f, 0.0f, 0.0f) };
    glm::vec3 h(0.0f, 0.0f, 10.0f); //�߶�����
    
    Loop loops[10];
    for (int i = 0; i < 8; i++) //����8�����涥��
    {
        loops[0].push_back(pts[i]); //����
        loops[1].push_back(pts[7 - i] + h); //����
        //����
        loops[i + 2].push_back(pts[i]);
        loops[i + 2].push_back(pts[i] + h);
        loops[i + 2].push_back(pts[(i + 1) % 8] + h);
        loops[i + 2].push_back(pts[(i + 1) % 8]);
    }
    //TODO:����10�������ɫ
    Color colors[10];
    for (int i = 0; i < 10; i++)
    {
        vector<Loop> ply_loops{loops[i]};
        Polygon polygon{ ply_loops, colors[i] };
        object.push_back(polygon);
    }        

    //3. ��Ⱦ
    //��ʼ����ɫ�������Ȼ���
    clearScreen();
    //��������
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