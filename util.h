#pragma once

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"

#include <iostream>
using namespace std;

ostream& operator<<(ostream& output, const glm::mat4& m)
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
ostream& operator<<(ostream& output, const glm::vec3& v)
{
	output << "[";
	for (int i = 0; i < 3; i++)
	{
		output << v[i] << " ";
	}
	output << "]";
	return output;
}
