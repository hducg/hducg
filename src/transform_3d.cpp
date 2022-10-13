#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"

#include <iostream>
#include <cmath>
using namespace std;

glm::vec3 center = glm::vec3(6.0f, 4.0f, 5.0f);
glm::vec3 camera = glm::vec3(-6.0f, 4.0f, 5.0f);

glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::lookAt(camera, center, glm::vec3(0.0f, 1.0f, 0.0f));
glm::mat4 projection = glm::mat4(1.0f);

glm::mat4 mvp = projection * view * model;

float angle = 30.0f;
unsigned int MatrixID, colorID;

ostream& operator<<(ostream &output, const glm::mat4 &m)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			output << m[j][i] << " ";
		output << endl;
	}
	return output;
}
ostream& operator<<(ostream& output, const glm::vec4& v)
{
	for (int i = 0; i < 4; i++)
	{
		output << v[i] << " ";	
	}
	output << endl;
	return output;
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{	
	bool update = false;
	if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
		update = true;
	}
	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
		update = true;
	}
	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{		
		model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
		update = true;
	}

	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		update = true;
	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		update = true;
	}
	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
	{
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
		update = true;
	}
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
	{
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		update = true;
	}
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
	{
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		update = true;
	}
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
	{
		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 0.0f));
		update = true;
	}

	if (key == GLFW_KEY_P && action == GLFW_PRESS)	//透视投影
	{
		projection = glm::perspective(glm::pi<float>() * 0.5f, 1.0f, 0.0f, 100.f);
		update = true;
	}
	if (key == GLFW_KEY_O && action == GLFW_PRESS)	//正交投影
	{
		projection = glm::orthoRH_NO(-12.0f, 12.0f, -12.0f, 12.0f, 6.0f, 100.0f);
		update = true;
	}
	if (update)
	{
		mvp = projection * view * model;
		cout << "model matrix = " << endl << model << endl;
		cout << "view matrix = " << endl << view << endl;
		cout << "projection matrix = " << endl << projection << endl;
		cout << "mvp matrix = " << endl << mvp << endl;
	}
}

void display(const glm::mat4 &m, const glm::vec4 &c, int mode, int first, unsigned count)
{
	//1.选择变换矩阵
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &m[0][0]);

	//2.选择颜色
	glUniform4fv(colorID, 1, &c[0]);

	//3.绘制    	
	glDrawArrays(mode, first, count);	
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
    std::cout << "Status: Using GLEW \n" << glewGetString(GLEW_VERSION) << std::endl;
    std::cout << "Status: Using OpenGL\n" << glGetString(GL_VERSION) << std::endl;
 
	//1. 创建着色器
	std::string vertexShader, fragmentShader;
	ParseShader("basic.shader", vertexShader, fragmentShader);
	//std::cout << "VERTEX"<<std::endl<<vertexShader << std::endl;
	//std::cout << "FRAGMENT" << std::endl << fragmentShader << std::endl;

	unsigned int shader = CreateShader(vertexShader, fragmentShader);
	glUseProgram(shader);

	//2. 定义顶点坐标
    float positions[] = {
		0,0,0,
		100,0,0,	//x轴
		0,0,0,
		0,100,0,	//y轴
		0,0,0,
		0,0,100,	//z轴
		0,0,0,
		12,0,0,
		12,8,0,
		0,8,0,		//前面
		0,0,10,
		12,0,10,
		12,8,10,
		0,8,10,		//后面
		0,0,0,
		0,8,0,
		0,8,10,
		0,0,10,		//左面
		12,0,0,
		12,8,0,
		12,8,10,
		12,0,10		//右面
    };

    //3. 为顶点申请GPU缓存
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    //选择缓存
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    //向缓存写入数据
    glBufferData(GL_ARRAY_BUFFER, 22 * 3 * sizeof(float), positions, GL_STATIC_DRAW);
	//说明存储结构
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	//启用索引为0的顶点属性，即坐标
    glEnableVertexAttribArray(0);
	
	//4. 变换矩阵	
	MatrixID = glGetUniformLocation(shader, "MVP");	

	//5.颜色
	colorID = glGetUniformLocation(shader, "COLOR");
	glm::vec4 color = glm::vec4(1.0f, 1.0f, 0.0f, 0.0f);
	glm::vec4 color_x = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	glm::vec4 color_y = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	glm::vec4 color_z = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);

    /* Loop until the user closes the window */	
    while (!glfwWindowShouldClose(window))
    {
		glClear(GL_COLOR_BUFFER_BIT);

        /* Render here */
		//绘制坐标轴
		display(mvp, color_x, GL_LINES, 0, 2);
		display(mvp, color_y, GL_LINES, 2, 2);
		display(mvp, color_z, GL_LINES, 4, 2);

		//绘制物体
		display(mvp, color, GL_LINE_LOOP, 6, 4);
		display(mvp, color, GL_LINE_LOOP, 10, 4);
		display(mvp, color, GL_LINE_LOOP, 14, 4);
		display(mvp, color, GL_LINE_LOOP, 18, 4);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}