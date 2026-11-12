#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 FragColor;

uniform mat4 projection;
uniform float uTime;
uniform int uObjectType;
uniform vec3 uBasePosition;

void main()
{
    vec3 pos = aPos;
    vec3 color = aColor;

    if (uObjectType == 0) {
        float angle = uTime * 1.5;
        float cosAngle = cos(angle);
        float sinAngle = sin(angle);

        float x =  pos.x * cosAngle + pos.z * sinAngle;
        float z = -pos.x * sinAngle + pos.z * cosAngle;
        pos.x = x;
        pos.z = z;

        pos += uBasePosition;
    } else if (uObjectType == 1) {
        pos += uBasePosition;
    }
    else if (uObjectType == 2) {
        vec3 offset = vec3(sin(uTime * 2.0f) * 0.7f, cos(uTime * 1.5f) * 0.4f, 0.0f);
        pos += uBasePosition + offset;
    } else if (uObjectType == 3) {
        pos += uBasePosition;
    }
    else if (uObjectType == 4) {
        float scale = 1.0f + 0.4f * sin(uTime * 3.0f);
        pos *= scale;
        pos+= uBasePosition;
    } else if (uObjectType == 5) {
        pos += uBasePosition;
    }

    gl_Position = projection * vec4(pos, 1.0f);
    FragColor = color;
}