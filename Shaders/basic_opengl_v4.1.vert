#version 410 core

layout (location = 0) in vec3 in_Position; 
layout (location = 1) in vec3 in_Color; 
out    	vec3 ex_Color; // Should be the same name in fragment shader

uniform mat4 u_proj_matrix;
uniform mat4 u_model_matrix;

void main(void) { 
    mat4 mvp_matrix = u_proj_matrix * u_model_matrix;
    gl_Position = mvp_matrix * vec4(in_Position, 1.0);
    gl_PointSize = 5.0f;
    ex_Color = in_Color; 
}

