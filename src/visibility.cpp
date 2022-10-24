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

//�������ĵ�����
glm::vec3 center = glm::vec3(7.5f, 7.5f, 5.0f);
//������������ĵľ���
float radius = 20.0f;
//��������Ǻͷ�λ�ǣ������������centerΪ���ġ��뾶Ϊradius���������ƶ�
float elevation = 0.0f, azimuth = 0.0f;
//�����ʼλ��
glm::vec3 camera = glm::vec3(center.x + radius * cos(elevation) * cos(azimuth),
	center.y + radius * cos(elevation) * sin(azimuth),
	center.z + radius * sin(elevation));

//��ͼ�任����
glm::mat4 view = glm::lookAt(camera, center, glm::vec3(0.0f, 0.0f, 1.0f));
//ͶӰ����
glm::mat4 projection = glm::perspective(glm::pi<float>() * 0.5f, 1.0f, 1.0f, 
	glm::length(center - camera) + 10.0f);

//������ɫ���б任���������ID
unsigned int MatrixID;
//�Ƿ�������Ȳ���
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
		glm::mat4 mvp = projection * view;
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
	}
}
void push_vertex(float* positions, int i, glm::vec3 pt)
{//���õ�i���������
	positions[i * 6] = pt.x;
	positions[i * 6 + 1] = pt.y;
	positions[i * 6 + 2] = pt.z;
}
void push_color(float* positions, int i, glm::vec3 color)
{//���õ�i�������ɫ
	positions[i * 6 + 3] = color.x;
	positions[i * 6 + 4] = color.y;
	positions[i * 6 + 5] = color.z;
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
 
	//1. ������ɫ��
	std::string vertexShader, fragmentShader;
	ParseShader("visibility.shader", vertexShader, fragmentShader);

	unsigned int shader = CreateShader(vertexShader, fragmentShader);
	glUseProgram(shader);
	
	//2. ��������
	//����8����
	glm::vec3 pts[8] = {
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 15.0f, 0.0f),
		glm::vec3(5.0f, 15.0f, 0.0f), glm::vec3(5.0f, 10.0f, 0.0f),
		glm::vec3(10.0f, 10.0f, 0.0f), glm::vec3(10.0f, 5.0f, 0.0f),
		glm::vec3(15.0f, 5.0f, 0.0f), glm::vec3(15.0f, 0.0f, 0.0f) };
	glm::vec3 h(0.0f, 0.0f, 10.0f); //�߶�����

	int num = 8 + 8 + 2 * 9;	//���µ����8���㣬����9���߸�2����
	float* positions = new float[num * 6];

	for (int i = 0; i < 8; i++) //����8�����涥��
	{
		//����
		push_vertex(positions, i, pts[i]);
		push_color(positions, i, glm::vec3(0.8f, 0.0f, 0.0f));

		//����
		push_vertex(positions, 8 + i, pts[i] + h);
		push_color(positions, 8 + i, glm::vec3(0.8f, 0.0f, 0.0f));
		
		//���
		push_vertex(positions, 16 + i * 2, pts[i]);
		push_color(positions, 16 + i * 2, glm::vec3(0.0f, 0.1f * i, 0.8f));

		push_vertex(positions, 17 + i * 2, pts[i] + h);
		push_color(positions, 17 + i * 2, glm::vec3(0.0f, 0.1f * i, 0.8f));
	}
	push_vertex(positions, 32, pts[0]);
	push_color(positions, 32, glm::vec3(0.0f, 0.0f, 0.8f));
	
	push_vertex(positions, 33, pts[0] + h);
	push_color(positions, 33, glm::vec3(0.0f, 0.0f, 0.8f));
    //3. Ϊ��������GPU����
    unsigned int VBO;
    glGenBuffers(1, &VBO);
	
    //ѡ�񻺴�
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //�򻺴�д������
    glBufferData(GL_ARRAY_BUFFER, num * 6 * sizeof(float), 
		positions, GL_STATIC_DRAW);
	
	//��������0������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 
		(void*)0);	
	glEnableVertexAttribArray(0);

	//��������1����ɫ
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 
		(void*)(3 * sizeof(float)));	
	glEnableVertexAttribArray(1);
	
	//4. �任����	
	MatrixID = glGetUniformLocation(shader, "MVP");
	glm::mat4 mvp = projection * view;
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
		glDrawArrays(GL_TRIANGLE_FAN, 0, 8);	//���������Ȼ��Ƶ���Ͷ���
		glDrawArrays(GL_TRIANGLE_FAN, 8, 8);	
		glDrawArrays(GL_QUAD_STRIP, 16, 18);	//���ı����������Ʋ���


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