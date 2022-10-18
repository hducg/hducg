#include <GL/glew.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include <ctime>
#include <cstdlib>
#include <vector>
using namespace std;

int width = 800, height = 800;
unsigned char foreground_color[3] = { 255,255,255 };
unsigned char background_color[3] = {0,0,0};
unsigned char* image = new unsigned char[width * height * 3];
vector<vector<pair<float, float>>> the_window, the_polygon, clipped_polygon;
vector<pair<float, float>> the_loop;

char draw_mode = 'w';
#define POLYGON 1
#define WINDOW 0
#define THRESHOLD 0.000001
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
    vertex* next[2] = {nullptr};
    bool entering = false, visited = false;

    vertex* insert(vertex* v, int which)
    {
        v->next[which] = this->next[which];
        this->next[which] = v;
        return v;
    }
    vertex* find_previous(int which)
    {
        //TODO:�ҵ���������ǰһ������
        //�������ʵ��
        return this;
    }
};

vertex* find_entering_vertex(const vector<vertex*>& loops)
{
    //TODO:�ҵ�loops��һ��δ�����ʹ�����㣬����Ҳ����ͷ��ؿ�
    //�������ʵ��
    return nullptr;
}
void intersect_edge_edge(vertex* v1, vertex* v2, vertex* w1, vertex* w2, float& tp, float& tw)
{
    //TODO:���v1v2��w1w2�Ľ���
    //������v1v2�ϵĲ���������tp��
    //������w1w2�ϵĲ���������tw��
}
float get_parameter(vertex* v1, vertex* v2, vertex* p)
{
    //TODO:����p���߶�v1v2�ϵĲ���ֵ
    return 0.0f;
}
void insert_between(vertex* v1, vertex* v2, vertex* p, int which)
{
    //TODO:��p���뵽��v1v2�ϣ�which��ʾWINDOW����POLYGON
    //v1v2֮����ܻ����������㣬��Ҫ�ҵ�һ�������v
    //v�Ĳ�����pС������v����һ������Ĳ�����p��
}

//������ʵ�ֶ���βü��㷨�����������clipped_polygon����
void WAClip(const vector<vector<pair<float, float>>>& window, const vector<vector<pair<float, float>>>& polygon, 
    vector<vector<pair<float, float>>>& clipped_polygon)
{
    //���ѭ�����������
    vector<vertex*> window_loops, polygon_loops;
    //������ж�������飬�ṩ�����ַ
    vector<vertex*> vertices;
    //��ų�ʼ�ߵ�����
    vector<pair<vertex*, vertex*>> window_edges, polygon_edges;
    //1. ����ѭ������
    //Ϊ���ڵ�ÿ��������ѭ������
    for (int i = 0; i < window.size(); i++)
    {
        //��һ������ָ���Լ�
        vertex* tmpV = new vertex;
        tmpV->x = window[i][0].first;
        tmpV->y = window[i][0].second;
        tmpV->next[WINDOW] = tmpV;
        vertices.push_back(tmpV);
        int first_idx = vertices.size() - 1;
        
        int last_idx;
        for (int j = 1; j < window[i].size(); j++)
        {
            vertex* tmpV = new vertex;
            tmpV->x = window[i][j].first;
            tmpV->y = window[i][j].second;            
            vertices.push_back(tmpV);
            last_idx = vertices.size() - 1;
            //�¶�����뵽ǰһ������֮��
            vertices[last_idx - 1]->insert(tmpV, WINDOW);
            window_edges.push_back(pair<vertex*, vertex*>(vertices[last_idx - 1],
                tmpV));
        }
        window_edges.push_back(pair<vertex*, vertex*>(vertices[last_idx],
            vertices[first_idx]));
        window_loops.push_back(vertices[first_idx]);
    }
    
    //Ϊ����ε�ÿ��������ѭ������
    for (int i = 0; i < polygon.size(); i++)
    {
        vertex* tmpV = new vertex;
        tmpV->x = polygon[i][0].first;
        tmpV->y = polygon[i][0].second;
        tmpV->next[POLYGON] = tmpV;
        vertices.push_back(tmpV);
        int first_idx = vertices.size() - 1;
        
        int last_idx;
        for (int j = 1; j < polygon[i].size(); j++)
        {
            vertex* tmpV = new vertex;
            tmpV->x = polygon[i][j].first;
            tmpV->y = polygon[i][j].second;
            
            vertices.push_back(tmpV);
            last_idx = vertices.size() - 1;
            vertices[last_idx - 1]->insert(tmpV, POLYGON);
            polygon_edges.push_back(pair<vertex*, vertex*>(vertices[last_idx - 1],
                vertices[last_idx]));
        }
        polygon_edges.push_back(pair<vertex*, vertex*>(vertices[last_idx],
            vertices[first_idx]));
        polygon_loops.push_back(vertices[first_idx]);
    }
    //2. �󽻵㣬��ѭ�������в��뽻��
    for (int i = 0; i < polygon_edges.size(); i++)
    {
        //��������ε����б�v1 ,v2
        vertex* v1 = polygon_edges[i].first;
        vertex* v2 = polygon_edges[i].second;

        //�������ڵ����б�w1 ,w2
        for (int j = 0; j < window_edges.size(); j++)
        {
            vertex* w1 = window_edges[j].first;
            vertex* w2 = window_edges[j].second;

            float tp, tw;
            intersect_edge_edge(v1, v2, w1,w2, tp, tw);

            //1. û�н���
            if (tp < -THRESHOLD || tp > 1 + THRESHOLD
                || tw < -THRESHOLD || tw > 1 + THRESHOLD)
            {
                //����
                continue;
            }

            vertex* v = nullptr, * w = nullptr;
            if (abs(tp) < THRESHOLD)   //��v1�غ�
                v = v1;
            if (abs(tp - 1) < THRESHOLD)   //��v2�غ�
                v = v2;
            if (abs(tw) < THRESHOLD)  //��w1�غ�
                w = w1;
            if (abs(tw - 1) < THRESHOLD)   //��w2�غ�
                w = w2;

            if (v != nullptr && w != nullptr)
            {
                //2. ����ʹ��ڼ�����ζ����غϣ���v�滻��w
                vertex* prev = v->find_previous(POLYGON);
                prev->next[POLYGON] = w;
                w->next[POLYGON] = v->next[POLYGON];
                continue;
            }
            else if (v != nullptr)
            {
                //3. ����Ͷ���ζ����غϣ���v���봰�ڱ�
                insert_between(w1, w2, v, WINDOW);
            }
            else if (w != nullptr)
            {
                //4. ����ʹ��ڶ����غϣ���w�������α�
                insert_between(v1, v2, w, POLYGON);
            }
            else
            {
                //5. pΪ��ͨ���㣬���봰�ڱߺͶ���α�
                vertex* p = new vertex;
                p->x = v1->x + tp * (v2->x - v1->x);
                p->y = v1->y + tp * (v2->y - v1->y);
                vertices.push_back(p);
                insert_between(w1, w2, p, WINDOW);
                insert_between(v1, v2, p, POLYGON);
            }
        }
    }

    //3. TODO:������ͳ���
    //�������ʵ��

    //4. ���������ռ��ü���Ķ���ζ���
    while(true)
    {
        vertex* start = find_entering_vertex(polygon_loops);
        if (start == nullptr)
            break;
        vertex* v = start->next[POLYGON];
        vector<pair<float, float>> loop;
        loop.push_back(pair<float, float>(start->x, start->y));
        while (v != start)
        {
            loop.push_back(pair<float, float>(v->x, v->y));
            if (v->entering)
            {
                v = v->next[POLYGON];
            }
            else
            {
                v = v->next[WINDOW];
            }
        }
        clipped_polygon.push_back(loop);
    }
    for (int i = 0; i < vertices.size(); i++)
        delete vertices[i];
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