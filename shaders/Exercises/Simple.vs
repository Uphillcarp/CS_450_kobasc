#version 430 core
// 410 for mac

layout(location=0) in vec3 position;
layout(location=1) in vec4 color;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;

out vec4 interColor;
out vec3 interPos;

void main() {
    vec4 pos = vec4(position, 1.0);
    vec4 viewpos = viewMat*modelMat*pos;
    vec4 projpos = projMat*viewpos;

    interColor = color;
    interPos = vec3(viewpos);

    gl_Position = projpos;
}