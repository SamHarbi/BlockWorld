/*
	This is a class that defines a cube, how to assemble a size*size*size big cube (also using perlin noise to give a terrain effect)
	out of the defined small cube and then finally drawing it with instancing.
	https://learnopengl.com/Advanced-OpenGL/Instancing Was used as a general guide (shows only for 2D) on how instance rendering works.
	Sameer Al Harbi 2022
*/
#pragma once

#include "wrapper_glfw.h"
#include "cube_tex.h"
#include <vector>

/* Include GLM core and matrix extensions*/
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

//Include Noise Function
# include "PerlinNoise.hpp"

class ChunkBlock
{
	public: 
		ChunkBlock();
		~ChunkBlock();

		void makeChunkBlock();
		void drawChunkBlock(int drawmode);
		int getChunkSize();
		void buildInstanceData(glm::vec3 position, int heightmod);

		glm::vec3 getTranslations(int i);

		// Define vertex buffer object names (e.g as globals)
		GLuint positionBufferObject;
		GLuint colourObject;
		GLuint normalsBufferObject;
		GLuint texCoordsObject;
		GLuint instanceData;

		GLuint attribute_v_coord; 
		GLuint attribute_v_normal;
		GLuint attribute_v_colours;
		GLuint attribute_v_instance;

		Cube instanceCube;

		int numvertices;
		int drawmode;
		int size; // size * size * size gives number of blocks

		//Positions at which each instance/small cube is draw in the larger chunk 
		std::vector<glm::vec3> translations;

		//Set seed for terrain generation 
		const siv::PerlinNoise::seed_type seed = 78948u;
		const siv::PerlinNoise perlin{ seed };
};
