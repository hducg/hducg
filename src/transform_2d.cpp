#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#include <iostream>

float angle1=0.0f, angle2 = 0.0f;
glm::mat4 mvp1 = glm::mat4(1.0f);   //矩形
glm::mat4 mvp2 = glm::mat4(1.0f);   //圆

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_O && action == GLFW_PRESS)
    {
        //计算新的圆心
        //用R(O)更新mvp1和mvp2
    }
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        //R(P)=T(P)*R(O)*T(-P), P是圆心
        //用R(P)更新mvp1
    }
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
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
    std::cout << "Status: Using GLEW \n" << glewGetString(GLEW_VERSION) << std::endl;
    std::cout << "Status: Using OpenGL\n" << glGetString(GL_VERSION) << std::endl;
    //1. 定义顶点坐标
    float positions[6] = {
        -0.5,-0.5,
        0.0,0.5,
        0.5,-0.5
    };

    //2. 申请缓存
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    //3. 指定当前缓存
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    //4. 指定数据
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);
    //5. 指定缓存结构
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8, 0);

    //6. 创建着色器
	std::string vertexShader, fragmentShader;
	ParseShader("basic.shader", vertexShader, fragmentShader);
	//std::cout << "VERTEX"<<std::endl<<vertexShader << std::endl;
	//std::cout << "FRAGMENT" << std::endl << fragmentShader << std::endl;

    unsigned int shader = CreateShader(vertexShader, fragmentShader);
    glUseProgram(shader);

	//7. 变换矩阵
		
	GLuint MatrixID = glGetUniformLocation(shader, "MVP");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
    /* Loop until the user closes the window */
	bool pressed = false;
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
             
		
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
		pressed = false;

        //8. 渲染    
        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}