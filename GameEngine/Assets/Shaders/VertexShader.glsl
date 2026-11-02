#version 330 core
layout (location = 0) in vec3 aPos;

uniform vec2 anchor;
uniform vec2 xAxis;
uniform vec2 yAxis;

void main()
{
    vec2 pos = anchor + aPos.x * xAxis + aPos.y * yAxis;
    gl_Position = vec4(pos, 0.0, 1.0);
}