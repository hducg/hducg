#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../shader.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"

#include <iostream>
#include <cmath>
#include <random>
using namespace std;

glm::vec3 center = glm::vec3(0.0f, 0.0f, -10.0f);
glm::vec3 camera = glm::vec3(0.0f, 0.0f, 0.0f);

glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::lookAt(camera, center, glm::vec3(0.0f, 1.0f, 0.0f));
glm::mat4 projection = glm::perspective(glm::pi<float>() * 0.5f, 1.0f, 1.0f, 
	glm::length(center - camera) * (1 + 1 / sqrt(2.0f)));

float elevation = 0.0f, azimuth = 0.0f;
unsigned int MatrixID;
bool depth_enabled = false;

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
	output << "[";
	for (int i = 0; i < 4; i++)
	{
		output << v[i] << " ";	
	}
	output << "]";
	return output;
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
		glm::mat4 mvp = projection * view * model;
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
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
    std::cout << "Status: Using GLEW \n" << glewGetString(GLEW_VERSION) << std::endl;
    std::cout << "Status: Using OpenGL\n" << glGetString(GL_VERSION) << std::endl;
 
	//1. 创建着色器
	std::string vertexShader, fragmentShader;
	ParseShader("visibility.shader", vertexShader, fragmentShader);

	unsigned int shader = CreateShader(vertexShader, fragmentShader);
	glUseProgram(shader);
	
	//2. 定义顶点坐标
	const int num = 20;	//三角形数量
	float* positions = new float[num * 18];
	std::random_device rd;
	std::default_random_engine eng(rd());
	std::uniform_real_distribution<float> 
		radius_sample(0.5f, glm::length(center - camera) / sqrt(2.0f)),
		theta_sample(0.0f, glm::pi<float>()),
		phi_sample(0.0f, 2 * glm::pi<float>()),
		color_sample;
	
	int idx = 0;
	for (int i = 0; i < num; i++)
	{
		//在半径为r球心为center的球面上随机取三个点构造一个三角形
		float radius = radius_sample(eng);
		float r = color_sample(eng);
		float g = color_sample(eng);
		float b = color_sample(eng);
		
		for (int j = 0; j < 3; j++)
		{
			float theta = theta_sample(eng);
			float phi = phi_sample(eng);
			
			positions[idx++] = center.x + radius * sin(theta) * cos(phi);
			positions[idx++] = center.y + radius * sin(theta) * sin(phi);
			positions[idx++] = center.z + radius * cos(theta);
			positions[idx++] = r;
			positions[idx++] = g;
			positions[idx++] = b;
		}
	}

    //3. 为顶点申请GPU缓存
    unsigned int VBO;
    glGenBuffers(1, &VBO);
	
    //选择缓存
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //向缓存写入数据
    glBufferData(GL_ARRAY_BUFFER, num * 18 * sizeof(float), 
		positions, GL_STATIC_DRAW);
	
	//顶点属性0：坐标
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 
		(void*)0);	
	glEnableVertexAttribArray(0);

	//顶点属性1：颜色
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 
		(void*)(3 * sizeof(float)));	
	glEnableVertexAttribArray(1);
	
	//4. 变换矩阵	
	MatrixID = glGetUniformLocation(shader, "MVP");
	glm::mat4 mvp = projection * view * model;
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
	std::cout << "view matrix = " << endl << view << endl;
	std::cout << "projection matrix = " << endl << projection << endl;
	
    /* Loop until the user closes the window */	
    while (!glfwWindowShouldClose(window))
    {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (depth_enabled)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);

        /* Render here */  	
		glDrawArrays(GL_TRIANGLES, 0, num * 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
	delete[] positions;
	
	glDeleteBuffers(1, &VBO);
    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}