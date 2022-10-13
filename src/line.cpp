#include <GL/glew.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include <ctime>
#include <cstdlib>

int width = 1024, height = 768;
unsigned char color[3] = {255, 0, 0};
unsigned char clear_color[3] = {0,0,0};
unsigned char* image = new unsigned char[width * height * 3];

void clear_screen(unsigned char c[3] = clear_color)
{
    int idx = 0;
    for (unsigned x = 0; x < width; x++)
    {
        for (unsigned y = 0; y < height; y++)
        {
            image[idx * 3] = c[0];
            image[idx * 3 + 1] = c[1];
            image[idx * 3 + 2] = c[2];
            idx++;
        }
    }
}
void  DDA_Line(unsigned x0,unsigned y0,unsigned x1,unsigned y1, 
    unsigned char c[3] = color)
{
    unsigned x = x0, y = y0; //直线上点的坐标

    //TODO:你的代码，需要能处理任意情况直线
    //用下面的代码将计算出的点信息存储到image
    int idx = y * width + x;
    image[idx * 3] = c[0];
    image[idx * 3 + 1] = c[1];
    image[idx * 3 + 2] = c[2];
}
void MidPoint_Line(unsigned x0, unsigned y0, unsigned x1, unsigned y1,
    unsigned char c[3] = color)
{
    unsigned x = x0, y = y0; //直线上点的坐标

    //TODO:你的代码，需要能处理任意情况直线
    //用下面的代码将计算出的点信息存储到image
    int idx = y * width + x;
    image[idx * 3] = c[0];
    image[idx * 3 + 1] = c[1];
    image[idx * 3 + 2] = c[2];
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

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        std::cout << "Error: \n", glewGetErrorString(err);
    }
    std::cout << "Status: Using GLEW \n" << glewGetString(GLEW_VERSION) << std::endl;
    std::cout << "Status: Using OpenGL\n" << glGetString(GL_VERSION) << std::endl;
 
    srand(int(time(0)));
    clear_screen();
    //随机构造1000条直线
    unsigned line[1000][4];
    for (int i = 0; i < 1000; i++)
    {
        line[i][0] = rand() % width;
        line[i][1] = rand() % height;
        line[i][2] = rand() % width;
        line[i][3] = rand() % height;
    }

    clock_t start = clock();
    for (int i = 0; i < 1000; i++)
    {
        DDA_Line(line[i][0],line[i][1],line[i][2],line[i][3]);
    }
    clock_t end = clock();
    std::cout << "DDA 算法耗时 " << (end - start) << "毫秒" << std::endl;
    
   start = std::clock();
    for (int i = 0; i < 1000; i++)
    {
        MidPoint_Line(line[i][0],line[i][1],line[i][2],line[i][3]);
    }
    end = std::clock();
    std::cout << "中点算法耗时 " << (end - start) << "毫秒" << std::endl;

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