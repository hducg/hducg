#include <GL/glew.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <list>
using namespace std;

int width = 800, height = 800;
unsigned char foreground_color[3] = { 255,255,255 };
unsigned char background_color[3] = {0,0,0};
unsigned char* image = new unsigned char[width * height * 3];
vector<vector<pair<float, float>>> the_polygon;
vector<pair<float, float>> the_loop;

//设置像素[x,y]的颜色
void plot(int x, int y, unsigned char c[3] = foreground_color)
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
    image[idx * 3] = c[0];
    image[idx * 3 + 1] = c[1];
    image[idx * 3 + 2] = c[2];
}

void clearScreen()
{
    for (unsigned x = 0; x < width; x++)
    {
        for (unsigned y = 0; y < height; y++)
        {
            plot(x,y, background_color);
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
    unsigned char c[3] = foreground_color)
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
            plot(real_y, x, c);
        else
            plot(x, real_y, c);

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

void fillInterval(int x1, int x2, int y)
{
    for (int x = x1; x <= x2; x++)
    {
        plot(x, y);
    }
}

void polygonFill(const vector<vector<pair<float, float>>>& polygon)
{
    //1. 初始化边表和活性边表
    vector<edge*> edge_table(height, nullptr);
    edge* active_edge_list = nullptr;
    int y = 0;
    //2. 遍历边表
    do
    {
        if (edge_table[y] != nullptr)
        {
            //1. 插入新边

        }
        //2. 配对，填充
        //3. 删除已到头的边
        y++;
    } while (active_edge_list != nullptr);
}

void drawPolygon(vector<vector<pair<float, float>>> polygon)
{
    for (int i = 0; i < polygon.size(); i++)
    {
        for (int j = 0; j < polygon[i].size(); j++)
        {
            int x0 = polygon[i][j].first + 0.5;
            int y0 = polygon[i][j].second + 0.5;
            int x1 = polygon[i][(j + 1) % polygon[i].size()].first + 0.5;
            int y1 = polygon[i][(j + 1) % polygon[i].size()].second + 0.5;
            MidPointLine(x0, y0, x1, y1);
            std::cout << "[" << x0 << "," << y0 << "]";
            if (j < polygon[i].size() - 1)
                std::cout << "-->";
        }
        std::cout << endl;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    //按鼠标左键添加顶点
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        the_loop.push_back(pair<float, float>(xpos, height - ypos));
        if (the_loop.size() > 1)
        {
            MidPointLine(the_loop[the_loop.size() - 2].first, the_loop[the_loop.size() - 2].second,
                the_loop[the_loop.size() - 1].first, the_loop[the_loop.size() - 1].second);
        }
    }
    //按鼠标右键封闭环
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        the_polygon.push_back(the_loop);
        //绘制最后一条边
        MidPointLine(the_loop[the_loop.size() - 1].first, the_loop[the_loop.size() - 1].second,
            the_loop[0].first, the_loop[0].second);
        the_loop.clear();   //清空数组
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //按回车键执行扫描填充
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
    {
        foreground_color[0] = 255;
        foreground_color[1] = foreground_color[2] = 0;
        polygonFill(the_polygon);
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
    
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        std::cout << "Error: \n", glewGetErrorString(err);
    }
    std::cout << "------------------Help------------------" << endl;
    std::cout << "Press key 'Enter': fill polygon" << endl;
    std::cout << "Press mouse left button: add vertex" << endl;
    std::cout << "Press mouse right button: close loop" << endl;

    //画背景
    clearScreen();
        
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        
        glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, image);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    delete image;

    glfwTerminate();
    return 0;
}