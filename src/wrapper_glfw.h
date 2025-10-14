/**
wrapper_glfw.h
Modified from the OpenGL GLFW example to provide a wrapper GLFW class
Iain Martin August 2014
*/
#pragma once

#include <string>

/* Inlcude GL_Load and GLFW */
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class GLWrapper {
private:

	int width;
	int height;
	const char *title;
	double fps;
	bool running;

public:
	GLWrapper(int width, int height, const char *title, void* rawbw);
	~GLWrapper();//

	void setFPS(double fps) {
		this->fps = fps;
	}

	void DisplayVersion();

	/* Callback registering functions */
	void setRenderer(void(*f)(void* bw));
	void setReshapeCallback(void(*f)(GLFWwindow* window, int w, int h));
	void setKeyCallback(void(*f)(GLFWwindow* window, int key, int scancode, int action, int mods));
	void setErrorCallback(void(*f)(int error, const char* description));
	void setMouseCallback(void(*func)(GLFWwindow* window, double xpos, double ypos));

	/* Shader load and build support functions */
	GLuint LoadShader(const char *vertex_path, const char *fragment_path);
	GLuint BuildShader(GLenum eShaderType, const std::string &shaderText);
	GLuint BuildShaderProgram(std::string vertShaderStr, std::string fragShaderStr);
	std::string readFile(const char *filePath);

	int eventLoop();
	GLFWwindow* getWindow();

	GLFWwindow* window;
	void(*renderer)(void* bw);

	void* bw;

};



