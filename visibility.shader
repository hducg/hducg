#shader vertex
#version 410 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;
out vec4 v_color;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(in_position,1.0f);
    v_color = vec4(in_color, 1.0f);
}

#shader fragment
#version 410 core

in vec4 v_color;
out vec4 out_color;

void main()
{
    out_color = v_color;
}