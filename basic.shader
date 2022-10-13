#shader vertex
#version 410 core

layout(location = 0) in vec4 position;
uniform mat4 MVP;

void main()
{
    gl_Position = MVP * position;
}

#shader fragment
#version 410 core

layout(location = 0) out vec4 out_color;
uniform vec4 COLOR;

void main()
{
    out_color = COLOR;
}