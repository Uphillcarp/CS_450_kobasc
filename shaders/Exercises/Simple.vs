#version 430 core

layout(location = 0) in vec3 position;

uniform mat4 modelMat;
uniform mat4 viewMat;

void main() {
    vec4 pos = vec4(position, 1.0);
    pos = viewMat*modelMat*pos;

    gl_Position = pos;
}