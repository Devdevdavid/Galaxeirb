/*
 * shader.c
 *
 *  Created on: Oct 6, 2018
 *      Author: David
 */

#include "shader.hpp"

GLuint vertexID;
GLuint fragmentID;
GLuint programID;

int32_t compile_shader(GLuint *shader, GLenum type, const char shaderFile[])
{
	FILE *fp = NULL;
	char * fileContent = NULL;
	const GLchar* sourceCodeStr;
	GLint compileError = 0;

	// Création du shader
	(*shader) = glCreateShader(type);

	// Vérification du shader
	if (shader == NULL) {
		log_error("Unable to create shader");
		return -1;
	}

	if ((fp = fopen(shaderFile, "r")) == NULL) {
		log_error("Unable to load shader file: %s", shaderFile);
		glDeleteShader((*shader));
		return -1;
	}

	fseek(fp, 0, SEEK_END);
	long fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	fileContent = (char *) malloc(fsize + 1);
	size_t result = fread(fileContent, fsize, 1, fp);
	if (result == 0 && ferror(fp)) {
		log_error("Unable to load shader file: %s", shaderFile);
		glDeleteShader((*shader));
		return -1;
	}
	fileContent[fsize] = '\0';
	fclose(fp);

	sourceCodeStr = fileContent;

	// Envoi du code source au shader
	glShaderSource((*shader), 1, &sourceCodeStr, 0);

	// Compilation du shader
	glCompileShader((*shader));

	free(fileContent);

	// Vérification de la compilation
	glGetShaderiv((*shader), GL_COMPILE_STATUS, &compileError);

	// S'il y a eu une erreur
	if (compileError != GL_TRUE) {
		GLint errorSize = 0;
		glGetShaderiv((*shader), GL_INFO_LOG_LENGTH, &errorSize);

		char errorStr[300];

		glGetShaderInfoLog((*shader), errorSize, &errorSize, errorStr);
		errorStr[errorSize-1] = '\0';

		log_error("Shader error log: {\n%s\n}", errorStr);

		log_error("Shader compilation error");
		glDeleteShader((*shader));
		return -1;
	}

	return 0;
}


int32_t load_shader(const char vertexSource[], const char fragmentSource[])
{
	if(glIsShader(vertexID) == GL_TRUE) {
		glDeleteShader(vertexID);
	}

	if(glIsShader(fragmentID) == GL_TRUE) {
		glDeleteShader(fragmentID);
	}

	if(glIsProgram(programID) == GL_TRUE) {
		glDeleteProgram(programID);
	}

	// Compilation des shaders
	if (compile_shader(&vertexID, GL_VERTEX_SHADER, vertexSource) != 0) {
		return -1;
	}

	if (compile_shader(&fragmentID, GL_FRAGMENT_SHADER, fragmentSource) != 0) {
		return -1;
	}

	// Create program
	programID = glCreateProgram();

	// Attach shaders
	glAttachShader(programID, vertexID);
	glAttachShader(programID, fragmentID);

	// Lock shader inputs
	glBindAttribLocation(programID, 0, "in_Vertex");
	glBindAttribLocation(programID, 1, "in_Color");
	glBindAttribLocation(programID, 2, "in_TexCoord0");

	// Link
	glLinkProgram(programID);

	// Check link
	GLint erreurLink = 0;
	glGetProgramiv(programID, GL_LINK_STATUS, &erreurLink);

	// If any error
	if (erreurLink != GL_TRUE) {
		glDeleteProgram(programID);
		return -1;
	}

	// Use program
	glUseProgram(get_shader_id());

	return 0;
}

void free_shaders(void)
{
	glDeleteShader(vertexID);
	glDeleteShader(fragmentID);
	glDeleteProgram(programID);
}

GLuint get_shader_id(void)
{
	return programID;
}

