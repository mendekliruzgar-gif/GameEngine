#version 330 core
in vec3 FragColor;
out vec4 FragColorOut;

uniform vec4 uColor;

void main()
{
    FragColorOut = vec4(FragColor, 1.0f);
} 