#version 410 core

in		vec3 ex_Color; // Should be the same name in vertex shader
out 	vec4 FragColor;

void main(void) { 
	FragColor = vec4(ex_Color, 1.0);
} 