#version 430 core

layout(location = 0) out vec4 out_color;

in vec4 interColor;
in vec3 interPos;

void main(){
    out_color = interColor; //vec4(1.0, 0.0, 0.0, 1.0);
}