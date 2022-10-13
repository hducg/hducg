#include <GL/glew.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include <ctime>
#include <cstdlib>
#include <vector>
using namespace std;

#define WINDOW 0
#define POLYGON 1

int width = 800, height = 800;
unsigned char foreground_color[3] = { 255,255,255 };
unsigned char background_color[3] = {0,0,0};
unsigned char* image = new unsigned char[width * height * 3];
vector<vector<pair<float, float>>> the_window, the_polygon, clipped_polygon;
vector<pair<float, float>> the_loop;

char draw_mode = 'w';

//��������[x,y]����ɫ
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

//����x��y��ֵ
void swap(int& x, int& y)
{
    int z = x;
    x = y;
    y = z;
}
void MidPointLine(int x0, int y0, int x1, int y1,
    unsigned char c[3] = foreground_color)
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
            plot(real_y, x, c);
        else
            plot(x, real_y, c);

        if (dm <= 0)    //�е���ֱ���Ϸ�
            dm = dm + dmp1;
        else
        {
            dm = dm + dmp2;
            y++;
        }
    }
}
class vertex
{
public:
    float x, y;
    bool entering = false;
    vertex* next[2] = { nullptr };

    vertex* insert(vertex* v, int which) //�ڴ˽ڵ������µĶ��㣬���ز���Ľڵ�ָ��
    {
        v->next[which] = this->next[which];
        this->next[which] = v;
        return v;
    }

    vertex* previous(int which)
    {
        vertex* prev = this;
        while (prev->next[which] != this)
            prev = prev->next[which];

        return prev;
    }
    vertex* remove(int which) //ɾ���˽ڵ㣬����ǰһ���ڵ�
    {
        vertex* prev = this->previous(which);
        prev->next[which] = this->next[which];

        if (prev == this)
            prev = nullptr;

        if (this->next[1 - which] == nullptr)
            delete this;
        return prev;
    }
};
void createVertexList(const vector<vector<pair<float, float>>>& loops, int which, vector<vertex*>& vertex_list)
{
    for (int i = 0; i < loops.size(); i++)
    {
        vertex* v = new vertex{ loops[i][0].first, loops[i][0].second };
        v->next[which] = v;
        vertex_list.push_back(v);
        for (int j = 1; j < loops[i].size(); j++)
        {
            v = v->insert(new vertex{ loops[i][j].first, loops[i][j].second }, which);
        }
    }
}
void clearVertexList(vector<vertex*>& v_list, int which)
{
    for (int i = 0; i < v_list.size(); i++)
    {
        vertex* v = v_list[i]->next[which];
    }
}
vertex* findEnteringVertex(const vector<vertex*>& polygon_list)
{
    return nullptr;
}
void WAClip(const vector<vector<pair<float, float>>>& window, const vector<vector<pair<float, float>>>& polygon,
    vector<vector<pair<float, float>>>& clipped_polygon)
{
    vector<vertex*> window_list, polygon_list;
    createVertexList(window, WINDOW, window_list);
    createVertexList(polygon, POLYGON, polygon_list);

    //1. �󽻵㲢��������
    //2. ��ǳ����
    //3. ����
    bool more = true;
    while (more)
    {
        vertex* start = findEnteringVertex(polygon_list);  //�ҵ�һ��δ��������
        vector<pair<float, float>> loop;

        vertex* v = start;
        do
        {
            loop.push_back(pair<float, float>(v->x, v->y));
            if (v->entering == true)
                v = v->next[POLYGON];
            else
                v = v->next[WINDOW];
        } while (v != start);
        clipped_polygon.push_back(loop);
    }
    clearVertexList(window_list, WINDOW);
    clearVertexList(polygon_list, POLYGON);
}

void drawPolygon(vector<vector<pair<float, float>>> polygon)
{
    for (int i = 0; i < polygon.size(); i++)
    {
        for (int j = 0; j < polygon[i].size(); j++)
        {
            float x0 = polygon[i][j].first;
            float y0 = polygon[i][j].second;
            float x1 = polygon[i][(j + 1) % polygon[i].size()].first;
            float y1 = polygon[i][(j + 1) % polygon[i].size()].second;
            MidPointLine(x0, y0, x1, y1);
            cout << "[" << x0 << "," << y0 << "]";
            if (j < polygon[i].size() - 1)
                cout << "-->";
        }
        cout << endl;
    }
}

void drawIcon()
{
    foreground_color[0] = foreground_color[1] = foreground_color[2] = 255;
    if (draw_mode == 'p')
        foreground_color[2] = 0;
    for (int x = 10; x < 40; x++)
        for (int y = height - 40; y < height - 10; y++)
            plot(x, y);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    //����������Ӷ���
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
    //������Ҽ���ջ�
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        //���ڻ���ģʽ�ѻ���ӵ���������
        if (draw_mode == 'w')
        {
            the_window.push_back(the_loop);
        }
        //����λ���ģʽ�ѻ���ӵ����������
        else if(draw_mode == 'p')
        {
            the_polygon.push_back(the_loop);
        }
        //�������һ����
        MidPointLine(the_loop[the_loop.size() - 1].first, the_loop[the_loop.size() - 1].second,
            the_loop[0].first, the_loop[0].second);
        the_loop.clear();   //�������
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //��'W'���л����ڻ���ģʽ
    if (key == GLFW_KEY_W && action == GLFW_PRESS)
    {
        if (!the_loop.empty())
            return;
        draw_mode = 'w';
        //�ð�ɫ���ƴ���
        foreground_color[0] = foreground_color[1] = foreground_color[2] = 255;
        drawIcon();
    }
    //��'P'���л�����λ���ģʽ
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        if (!the_loop.empty())
            return;
        draw_mode = 'p';
        //�û�ɫ���ƶ����
        foreground_color[0] = foreground_color[1] = 255;
        foreground_color[2] = 0;
        drawIcon();
    }
    //���س���ִ�вü�
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
    {
        WAClip(the_window, the_polygon, clipped_polygon);
        //�ú�ɫ���Ʋü���Ķ����
        foreground_color[0] = 255;
        foreground_color[1] = foreground_color[2] = 0;
        drawPolygon(clipped_polygon);
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
    cout << "------------------Help------------------" << endl;
    cout << "Press key 'W': draw window" << endl;
    cout << "Press key 'P': draw polygon" << endl;
    cout << "Press key 'Enter': clip polygon" << endl;
    cout << "Press mouse left button: add vertex" << endl;
    cout << "Press mouse right button: close loop" << endl;

    //������
    clearScreen();
    //��ͼ��
    drawIcon();
        
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