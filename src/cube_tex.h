/* cube_tex.h
Example class to to show a cube implementation with texture coords
Iain Martin November 2018
*/

#pragma once

#include "wrapper_glfw.h"
#include <vector>
#include <glm/glm.hpp>

class Cube
{
public:
	Cube(bool useTexture=false);
	~Cube();

	void makeCube();
	void drawCube(int drawmode);

	// Define vertex buffer object names (e.g as globals)
	GLuint positionBufferObject;
	GLuint colourObject;
	GLuint normalsBufferObject;
	GLuint texCoordsObject;

	GLuint attribute_v_coord;
	GLuint attribute_v_normal;
	GLuint attribute_v_colours;
	GLuint attribute_v_texcoord;

	int numvertices;
	int drawmode;
	bool enableTexture;
};
