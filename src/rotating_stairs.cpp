#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "gtx/transform.hpp"
#include "shader.h"

#include <iostream>
#include <vector>
using namespace std;

glm::mat4 model = glm::mat4(1.0f);

glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 camera = glm::vec3(0.0f, 0.0f, 350.0f);
glm::mat4 view = glm::lookAt(camera, center, glm::vec3(0.0f, 1.0f, 0.0f));

//glm::mat4 projection = glm::orthoRH_NO(-180.0f, 180.0f, -180.0f, 180.0f, 0.0f, 400.0f);
glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.0f, 400.0f);

vector<pair<glm::mat4, float>> stairs;
float intensity = 1.0;
unsigned int COLOR_ID, MATRIX_ID;
glm::vec4 color = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
bool facet = false;

void display(const glm::mat4& m, const glm::vec4& c, int mode, unsigned count)
{
    //1.选择变换矩阵
    glUniformMatrix4fv(MATRIX_ID, 1, GL_FALSE, &m[0][0]);
    //2.选择颜色
    glUniform4fv(COLOR_ID, 1, &c[0]);
    //3.绘制
    glDrawElements(mode, count, GL_UNSIGNED_INT, nullptr);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_S && action == GLFW_PRESS)
    {
        model = glm::rotate(glm::radians(12.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * model;
        model = glm::translate(glm::vec3(0.0f, 0.0f, 10.0f)) * model;
        intensity = intensity * 0.95;
        stairs.push_back(pair<glm::mat4, float>(model, intensity));
    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        color = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
    }
    if (key == GLFW_KEY_G && action == GLFW_PRESS)
    {
        color = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
    }
    if (key == GLFW_KEY_B && action == GLFW_PRESS)
    {
        color = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
    }
    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
    {        
        camera += glm::vec3(0.0f, 5.0f, 0.0f);
        view = glm::lookAt(camera, center, glm::vec3(0.0f, 1.0f, 0.0f));
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
    {
        camera += glm::vec3(0.0f, -5.0f, 0.0f);
        view = glm::lookAt(camera, center, glm::vec3(0.0f, 1.0f, 0.0f));
    }
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
    {
        camera += glm::vec3(-5.0f, 0.0f, 0.0f);
        view = glm::lookAt(camera, center, glm::vec3(0.0f, 1.0f, 0.0f));
    }
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
    {
        camera += glm::vec3(5.0f, 0.0f, 0.0f);
        view = glm::lookAt(camera, center, glm::vec3(0.0f, 1.0f, 0.0f));
    }
    if (key == GLFW_KEY_F && action == GLFW_PRESS)
    {
        facet = true;
    }
    if (key == GLFW_KEY_W && action == GLFW_PRESS)
    {
        facet = false;
    }
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(800, 800, "Hello World", NULL, NULL);
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

    //0. 创建着色器
    std::string vertexShader, fragmentShader;
    ParseShader("basic.shader", vertexShader, fragmentShader);
    unsigned int shader = CreateShader(vertexShader, fragmentShader);
    glUseProgram(shader);
    //获取着色器中uniform变量的id
    COLOR_ID = glGetUniformLocation(shader, "COLOR");    
    MATRIX_ID = glGetUniformLocation(shader, "MVP"); 
    
    stairs.push_back(pair<glm::mat4, float>(model, intensity));

    //1. 定义顶点坐标
    float positions[] = {
        50, 20, 0,      //0
        160, 20, 0,     //1
        160, -20, 0,    //2
        50, -20, 0,     //3
        50, 20, 10,     //4
        160, 20, 10,    //5
        160, -20, 10,   //6
        50, -20, 10,    //7
    };
    unsigned int indices[] = {
        0, 1,
        1, 2,
        2, 3,
        3, 0,
        4, 5,
        5, 6,
        6, 7,
        7, 4,
        0, 4,
        1, 5,
        2, 6,
        3, 7
    };
    unsigned int facet_indices[] = {
        0, 1, 2,    //bottom
        0, 2, 3,
        4, 7, 6,    //top
        4, 6, 5,
        0, 4, 5,    //back
        0, 5, 1,
        3, 2, 6,    //front
        3, 6, 7,
        0, 7, 4,    //left
        0, 3, 7,
        1, 5, 6,    //right
        1, 6, 2
    };
    //2. 创建顶点缓存
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 8 * 3 * sizeof(float), positions, GL_STATIC_DRAW);

    //3. 指定缓存结构
    glEnableVertexAttribArray(0);
    //void glVertexAttribPointer(GLuint	        index,
    //                           GLint	        size,
    //                           GLenum	        type,
    //                           GLboolean	    normalized,
    //                           GLsizei	    stride,
    //                           const GLvoid * pointer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

    //4. 创建索引缓存
    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 12 * 2 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    unsigned int facet_ibo;
    glGenBuffers(1, &facet_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, facet_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 12 * 3 * sizeof(unsigned int), facet_indices, GL_STATIC_DRAW);

    /* Loop until the user closes the window */
    double time = glfwGetTime();
    glm::mat4 rotate_z = glm::mat4(1.0f);
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        if ((glfwGetTime() - time) > 0.04)
        {
            time = glfwGetTime();
            rotate_z = glm::rotate(glm::radians(1.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * rotate_z;
        }
                
        for (int i = 0; i < stairs.size(); i++)
        {        
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, facet_ibo);
            display(projection * view * rotate_z * stairs[i].first, color * stairs[i].second, GL_TRIANGLES, 36);
            
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
            display(projection * view * rotate_z * stairs[i].first, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), GL_LINES, 24);                       
        }
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}