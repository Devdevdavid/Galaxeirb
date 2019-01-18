#version 120 
attribute  vec4 in_Position; 
attribute  vec3 in_Color; 
varying    vec3 ex_Color; 

uniform mat4 u_proj_matrix;
uniform mat4 u_model_matrix;

void main(void) { 
    mat4 mvp_matrix = u_proj_matrix * u_model_matrix;
    gl_Position = mvp_matrix * in_Position;
    gl_PointSize = 1.5f;
    ex_Color = in_Color; 
}

