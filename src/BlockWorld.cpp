/*
 Adapted From Lab5 Solution (Texturing)
 Iain Martin November 2022

 Manages the creation and generation of a 3D Voxel based outdoor scene with all models and terrain textured. Creates a textured Skybox too.
 Sameer Al Harbi 2022

 !NOTE! This project needs to be compiled as per C++17 or C++20 Standard to work.
 This can be set in visual studio by going to Project > Properties > C/C++ > Language > C++ Language Standard
*/

/* Link to static libraries, could define these as linker inputs in the project settings instead
if you prefer */
#ifdef _DEBUG
#pragma comment(lib, "glfw3D.lib")
#else
#pragma comment(lib, "glfw3.lib")
#endif
#pragma comment(lib, "opengl32.lib")

/* Include the header to the GLFW wrapper class which
   also includes the OpenGL extension initialisation*/
#include "wrapper_glfw.h"
#include <iostream>

/* Include GLM core and matrix extensions*/
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

/* Include the image loader */
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Object loader classes
#include "ModelLoader/tiny_loader_texture.h"

//Perlin Noise for random gen - MIT Licensed Library on github : https://github.com/Reputeless/PerlinNoise
# include "PerlinNoise.hpp"

/* Include Objects to build our scene */
#include "ChunkBlock.h"

/* Stack Data Structure */
#include <stack>

const int numOfPrograms = 3; //How many programs will be used
GLuint program[numOfPrograms];		/* Identifiers for the shader prgorams */
GLuint vao;			/* Vertex array (Containor) object. This is the index of the VAO that will be the container for
					   our buffer objects */

int colourmode;

					  /* Position and view globals */
GLfloat angle_x, angle_inc_x, x, scaler, z, y;
GLfloat angle_y, angle_inc_y, angle_z, angle_inc_z;

//Camera Controls
double horizontalCam; 
double verticalCam;
GLfloat cam_x;
GLfloat cam_y;
GLfloat cam_z;

//Perlin Settings controllable by user 
int heightmod; //height of terrain 

//Camera Position Incrementals 
GLfloat cam_x_mod;
GLfloat cam_y_mod;
GLfloat cam_z_mod;

/* Uniforms*/
GLuint modelID[numOfPrograms], viewID[numOfPrograms], projectionID[numOfPrograms];
int colourmodeID[numOfPrograms];
GLuint lightviewID[2];
GLuint drawmode;			// Defines drawing mode as points, lines or filled polygons
GLfloat aspect_ratio;		/* Aspect ratio of the window defined in the reshape callback*/
GLuint normalMatrixID;

//Texture IDs
GLuint AtlasID, GrassTextureID, SkyTextureID;

//Tree Models to render and skybox cube
TinyObjLoader tree1, tree2;
Cube cube(true);

ChunkBlock chunkblock; //Single 16x16x16 Chunk Block
glm::vec3 megaChunk[9]; //Positions of all visible Chunks around a player
glm::vec3 chunkOrigin; //Origin Point of first chunk where player starts

// Define the normal matrix used by Trees lightning
glm::mat3 normalmatrix;

using namespace std;
using namespace glm;

//Define methods for compiler 
bool load_texture(char* filename, GLuint& texID, bool bGenMipmaps);
bool loadCubeMap(GLuint& texID, vector<std::string> faces);

//Menu of Controls
void Menu()
{
	cout << "+--------[ Block World ]--------+ " << endl;
	cout << "Use Mouse to aim and W Or S to move forward towards or away from aiming direction respectively" << endl;
	cout << "N Changes rendering mode from triangles, to Lines, to Points and back again" << endl;
	cout << "Use M to change diffuse colors" << endl;
	cout << "Use H to cycle Height Modifier of terrain to a maximum value" << endl;
	cout << "" << endl;
}

//Extensively uses code from https://learnopengl.com/Advanced-OpenGL/Cubemaps
// Cube Map texture Loader / loads each face in faces vector. Returns true if texture loaded successfully
bool loadCubeMap(GLuint& texID, vector<std::string> faces)
{
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

	// local image parameters
	int width, height, nrChannels;

	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			//Figure out what pixel format image is in
			int pixel_format = 0;
			if (nrChannels == 3)
				pixel_format = GL_RGB;
			else
				pixel_format = GL_RGBA;
			
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, pixel_format, width, height, 0, pixel_format, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
			return false;
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	return true;
}

//Regular Texture Loader - One Image. Returns true if texture loaded successfully
bool load_texture(const char* filename, GLuint& texID, bool bGenMipmaps)
{
	glGenTextures(1, &texID);
	// local image parameters
	int width, height, nrChannels;

	/* load an image file using stb_image */
	unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);

	// check for an error during the load process
	if (data)
	{
		// Note: this is not a full check of all pixel format types, just the most common two!
		int pixel_format = 0;
		if (nrChannels == 3)
			pixel_format = GL_RGB;
		else
			pixel_format = GL_RGBA;

		// Bind the texture ID before the call to create the texture.
			// texID[i] will now be the identifier for this specific texture
		glBindTexture(GL_TEXTURE_2D, texID);

		// Create the texture, passing in the pointer to the loaded image pixel data
		glTexImage2D(GL_TEXTURE_2D, 0, pixel_format, width, height, 0, pixel_format, GL_UNSIGNED_BYTE, data);

		// Generate Mip Maps
		if (bGenMipmaps)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			// If mipmaps are not used then ensure that the min filter is defined
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}
	}
	else
	{
		printf("stb_image  loading error: filename=%s", filename);
		return false;
	}
	stbi_image_free(data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	return true;
}

//Generate positions at which chunks need to be drawn 
void generateMegaChunk(bool origin, glm::vec3 direction)
{
	/*
		Mega Chunk Structure
		|___| = One Chunk
		|_x_| = Middle Chunk spawned at cam position and used to calculate rest of Mega Chunk relatively if orgin = true
	
		1|___|2|___|3|___|
		4|___|5|_x_|6|___|
		7|___|8|___|9|___|

		When generating new mega chunks as the player is moving, instead of spawing a new chunk at camera position instead get 
		the direction the player is moving and move the megachunk respectively, unchanged chunks are regenerated this way which could be improved 
		chunks use perlin noise based on world position so the terrain looks continous irrespective of how chunks movement and regeneration
	*/
	
	int chunkSize = chunkblock.getChunkSize(); //Get size of a single chunk, default is defined as 16
	glm::vec3 ip = glm::vec3(0, 0, 0); 
	
	if (origin == true)
	{
		ip = glm::vec3(cam_x - chunkSize / 2, -20, cam_z - chunkSize / 2); //Calculate where the new middle chunk should be based on cam positon
		chunkOrigin = glm::vec3(ip.x, ip.y, ip.z);
	}
	else
	{
		ip = glm::vec3(chunkOrigin.x + direction.x, chunkOrigin.y + direction.y, chunkOrigin.z + direction.z); //Move whole mega chunk towards a direction
		chunkOrigin = glm::vec3(ip.x, ip.y, ip.z);
	}
		
	//Define actual positions of chunks 
	megaChunk[0] = glm::vec3(ip.x + chunkSize, ip.y, ip.z + chunkSize); //1
	megaChunk[1] = vec3(ip.x, ip.y, ip.z + chunkSize); //2
	megaChunk[2] = vec3(ip.x - chunkSize, ip.y, ip.z + chunkSize); //3

	megaChunk[3] = vec3(ip.x + chunkSize, ip.y, ip.z); //4
	megaChunk[4] = vec3(ip.x, ip.y, ip.z); //5
	megaChunk[5] = vec3(ip.x - chunkSize, ip.y, ip.z); //6

	megaChunk[6] = vec3(ip.x + chunkSize, ip.y, ip.z - chunkSize); //7
	megaChunk[7] = vec3(ip.x, ip.y, ip.z - chunkSize); //8
	megaChunk[8] = vec3(ip.x - chunkSize, ip.y, ip.z - chunkSize); //9
}

/*
This function is called before entering the main rendering loop.
Use it for all your initialisation stuff
*/
void init(GLWrapper* glw)
{
	/* Set the object transformation controls to their initial values */

	//Initial Terrain Position 
	x = 0;
	y = 0;
	z = 0;

	angle_x = angle_y = angle_z = 0;
	angle_inc_x = angle_inc_y = angle_inc_z = 0;
	scaler = 1.f;
	aspect_ratio = 1.3333f;
	colourmode = 0;

	horizontalCam = 0.0f;
	verticalCam = 0.0f;

	//Initial Camera Positon
	cam_x = 13;
	cam_y = 0;
	cam_z = 13;

	heightmod = 10;

	cout << "About to gen vertex arrays" << endl;

	// Generate index (name) for one vertex array object
	glGenVertexArrays(1, &vao);

	// Create the vertex array object and make it current
	glBindVertexArray(vao);

	cout << "About to make cubes" << endl; //

	/* Create a Chunk Block and Skybox Cube */
	cube.makeCube(); //
	chunkblock.makeChunkBlock();

	cout << "loading models.." << endl;

	//Load in and create custom imported 3D Models
	/*
		These Models have been purchased as part of the POLYGON - Adventure Pack from the Synty Store
		The Pack: https://syntystore.com/products/polygon-adventure-pack?_pos=1&_sid=19b8ccc9f&_ss=r
	*/
	tree1.load_obj("Models/SM_Env_TreePine_03.obj");
	tree2.load_obj("Models/SM_Env_Tree_01.obj");

	cout << "models loaded" << endl; //

	//Create initial terrain megachunk positions using inital position
	generateMegaChunk(true, chunkOrigin);

	// This is the location of the texture object (TEXTURE0), i.e. tex1 will be the name
	// of the sampler in the fragment shader
	int loc;

	//For each program in numOfPrograms
	for (int i = 0; i < numOfPrograms; i++)
	{
		try
		{
			cout << "loading shaders.." << endl;
			//Build path for shaders based on which program is being loaded
			/*
				program[i] -> Built from program_v_i.vert + program_f_i.frag
			*/
			string path_v = "Shaders/program_v_" + to_string(i) + ".vert";
			string path_f = "Shaders/program_f_" + to_string(i) + ".frag";
			program[i] = glw->LoadShader(&path_v[0], &path_f[0]);
			cout << "done loading shaders.." << endl;
		}
		catch (exception& e)
		{
			cout << "Caught exception: " << e.what() << endl;
			cin.ignore();
			exit(0);
		}//

		/* Define uniforms to send to vertex shader */
		modelID[i] = glGetUniformLocation(program[i], "model");
		colourmodeID[i] = glGetUniformLocation(program[i], "colourmode");
		viewID[i] = glGetUniformLocation(program[i], "view");
		projectionID[i] = glGetUniformLocation(program[i], "projection");

		loc = glGetUniformLocation(program[i], "tex1");
		if (loc >= 0) glUniform1i(loc, 0);
	}

	//Uniform that's only for shader program 0 & 2 - Terrain & Trees
	lightviewID[0] = glGetUniformLocation(program[0], "light_view");
	lightviewID[1] = glGetUniformLocation(program[2], "light_view");

	//Uniform that's only for shader program 2 - Trees
	normalMatrixID = glGetUniformLocation(program[2], "normalmatrix");

	//Define texture images that make a terrain block cubemap texture
	/*
		These textures are from Kenny Game Assets, Voxel Pack available here: https://www.kenney.nl/assets/voxel-pack
	*/
	vector<std::string> faces
	{
			"Grassblock/dirt_grass.png",
			"Grassblock/dirt_grass.png",
			"Grassblock/grass_top.png",
			"Grassblock/dirt.png",
			"Grassblock/dirt_grass.png",
			"Grassblock/dirt_grass.png"
	};

	//Skybox cubemap
	/*
		These skybox textures are from OpenGameArt uploaded by Spiney as Cloudy Skyboxes, Available Here: https://opengameart.org/content/cloudy-skyboxes
	*/
	vector<std::string> faces_back
	{
			"Skybox/bluecloud_ft.jpg",
			"Skybox/bluecloud_bk.jpg",
			"Skybox/bluecloud_up.jpg",
			"Skybox/bluecloud_dn.jpg",
			"Skybox/bluecloud_rt.jpg",
			"Skybox/bluecloud_lf.jpg"
	};

	if (!loadCubeMap(GrassTextureID, faces))
	{
		cout << "Fatal error loading Grass Cubemap" << endl;
		exit(0);
	}

	if (!loadCubeMap(SkyTextureID, faces_back))
	{
		cout << "Fatal error loading Grass Cubemap" << endl;
		exit(0);
	}

	stbi_set_flip_vertically_on_load(true);

	//Load in Atlas Texture for models - Was distributed with models (See above)
	const char* atlasfilename = "PolyAdventureTexture_01.png";

	if (!load_texture(atlasfilename, AtlasID, true))
	{
		cout << "Fatal error loading Atlas Texture" << endl;
		exit(0);
	}

	Menu(); //Display Controls Menu

}

/*
	Display subfunction that handles rendering trees (program 2)
*/
void display_Trees(mat4 view, mat4 lightview, mat4 projection, TinyObjLoader tree, TinyObjLoader alt_tree)
{
	glUseProgram(program[2]);

	/* Enable depth test  */
	glEnable(GL_DEPTH_TEST);
	glDepthMask(true);

	//To correctly render these models in particular 
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	// Define our model transformation in a stack and 
	// push the identity matrix onto the stack
	stack<mat4> model;
	model.push(mat4(1.0f));

	// Send our uniforms variables to the currently bound shader,
	glUniform1i(colourmodeID[2], colourmode);
	glUniformMatrix4fv(viewID[2], 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projectionID[2], 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(lightviewID[1], 1, GL_FALSE, &lightview[0][0]);

	//Bind Texture
	glBindTexture(GL_TEXTURE_2D, AtlasID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	//Render 5 Trees 
	for (int i = 1; i < 5; i++)
	{
		//Get Position of a top block of previously rendered chunk
		vec3 pos = chunkblock.getTranslations((14 + 15 * 16) * (i * 2));

		model.push(model.top());
		{
			model.top() = translate(model.top(), vec3(pos.x, pos.y, pos.z));
			model.top() = scale(model.top(), vec3(0.01, 0.01, 0.01));
			glUniformMatrix4fv(modelID[2], 1, GL_FALSE, &(model.top()[0][0]));

			// Recalculate the normal matrix and send to the vertex shader
			normalmatrix = transpose(inverse(mat3(lightview * model.top())));
			glUniformMatrix3fv(normalMatrixID, 1, GL_FALSE, &normalmatrix[0][0]);

			//Draw one of two tree models 
			if (i > 2)
			{
				alt_tree.drawObject(drawmode);
			}
			else
			{
				tree.drawObject(drawmode);
			}
		}
		model.pop();
	}

	glCullFace(GL_BACK);

}

/*
	Display subfunction that handles rendering terrain (program 0)
*/
void display_Terrain(mat4 view, mat4 lightview, vec3 camPos, vec3 camDirection, mat4 projection)
{
	/* Enable depth test  */
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	/* Make the compiled shader program current */
	glUseProgram(program[0]);

	// Define our model transformation in a stack and 
	// push the identity matrix onto the stack
	stack<mat4> model;
	model.push(mat4(1.0f));

	// Send our uniforms variables to the currently bound shader,
	glUniform1i(colourmodeID[0], colourmode);
	glUniformMatrix4fv(viewID[0], 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projectionID[0], 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(lightviewID[0], 1, GL_FALSE, &lightview[0][0]);

	model.top() = scale(model.top(), vec3(2.0f, 2.0f, 2.0f));//scale equally in all axis

	//Check if camera position is approaching megachunks bounds, if so- then regenerate for the new camera position
	//X Bounds
	if (cam_x > 32 + megaChunk[5].x)
	{
		generateMegaChunk(false, glm::vec3(16, 0, 0));
	}
	else if (cam_x < megaChunk[5].x + 16)
	{
		glm::vec3 dir = glm::vec3(-16, 0, 0);
		generateMegaChunk(false, dir);
	}
	//Z Bounds
	if (cam_z > 16 + megaChunk[5].z)
	{
		generateMegaChunk(false, glm::vec3(0, 0, 16));
	}
	else if (cam_z < megaChunk[5].z)
	{
		generateMegaChunk(false, glm::vec3(0, 0, -16));
	}

	//Bind Grass Block texture
	glBindTexture(GL_TEXTURE_CUBE_MAP, GrassTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	//9 as in 9 locations defined in megachunk 
	for (int i = 0; i < 9; i++)
	{
		model.push(model.top());
		{
			model.top() = translate(model.top(), vec3(x, y, z));
			glUniformMatrix4fv(modelID[0], 1, GL_FALSE, &(model.top()[0][0]));

			cout << "Drawing Chunk 0" << endl;
			chunkblock.buildInstanceData(megaChunk[i], heightmod); //Build a Chunk at position set out in megachunk
			cout << "Drawing Chunk 1" << endl;
			chunkblock.drawChunkBlock(drawmode); //Draw that chunk

			display_Trees(view, lightview, projection, tree1, tree2); //Render tree's for that chunk

			glUseProgram(program[0]); //After tree rendering is done, prepare to render next chunk
		}
		model.pop();
	}
}

/*
	Display subfunction that handles rendering the skybox (program 1)
*/
void display_SkyBox(vec3 up, vec3 camPos, vec3 camDirection, mat4 projection)
{
	/* Make the compiled shader program current */
	glUseProgram(program[1]);

	// Define our model transformation in a stack and 
	// push the identity matrix onto the stack
	stack<mat4> model;
	model.push(mat4(1.0f));

	//Don't Cull Faces
	glDisable(GL_CULL_FACE);

	// Camera matrix - This one is locked to always be at 0, 0, 0 and ignore camera movement 
	mat4 view = lookAt(
		vec3(0, 0, 0), //locked position
		vec3(0, 0, 0) + camDirection,
		up
	);

	cout << "skybox 0" << endl;

	// Send our uniforms variables to the currently bound shader,
	glUniform1ui(colourmodeID[1], colourmode);
	glUniformMatrix4fv(viewID[1], 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projectionID[1], 1, GL_FALSE, &projection[0][0]);

	cout << "skybox 1" << endl;

	//Bind Skybox texture
	glBindTexture(GL_TEXTURE_CUBE_MAP, SkyTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	cout << "skybox bound" << endl;

	model.push(model.top());
	{
		model.top() = scale(model.top(), vec3(200, 200, 200));
		model.top() = translate(model.top(), vec3(0, 0, 0));
		glUniformMatrix4fv(modelID[1], 1, GL_FALSE, &(model.top()[0][0]));

		cout << "going to draw cube" << endl;
		cube.drawCube(drawmode);
		cout << "cube drawn" << endl;
	}
	model.pop();

	cout << "skybox done" << endl;

}


/* Called to update the display. Note that this function is called in the event loop in the wrapper
   class because we registered display as a callback function */
void display()
{
	cout << "callback stuff worked" << endl;
	glfwSetTime(0);
	
	/* Define the background colour */
	glClearColor(102.0f/255.0f, 153.0f/255.0f, 255.0f/255.0f, 1.0f);

	/* Clear the colour and frame buffers */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Create vec3 from camera position components 
	vec3 camPos = vec3(cam_x, cam_y, cam_z);

	//Camera Direction
	vec3 camDirection = vec3(cos(verticalCam) * sin(horizontalCam), sin(verticalCam), cos(verticalCam) * cos(horizontalCam));

	// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	mat4 projection = perspective(radians(90.0f), aspect_ratio, 0.1f, 100.0f);

	//Used to calculate the correct Head up position
	vec3 right = vec3(
		sin(horizontalCam - 3.14f / 2.0f),
		0,
		cos(horizontalCam - 3.14f / 2.0f)
	);

	//Get which way is up 
	vec3 up = glm::cross(right, camDirection);

	// Camera matrix
	mat4 view = lookAt(
		camPos,
		camPos + camDirection,
		up
	);

	//Used to calculate light 
	mat4 lightview = lookAt(
		vec3(-8, 4, 2),
		vec3(1, 0, 1),
		vec3(1, 1, 1)
	);

	//Call display subfunctions that render each part of the scene with different shader programs and other variations
	cout << "Camera and background setup worked" << endl;

	display_SkyBox(up, camPos, camDirection, projection);

	cout << "Skybox rendered" << endl;

	display_Terrain(view, lightview, camPos, camDirection, projection);

	cout << "Terrian rendered" << endl;

	// Disable everything
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisableVertexAttribArray(0);
	glUseProgram(0);

	//Which way the camera is looking becomes the vector towards which the camera moves forward
	cam_x_mod = camDirection.x;
	cam_y_mod = camDirection.y;
	cam_z_mod = camDirection.z;
}

/*
	Function to calculate camera angles based on mouse input
	Uses http://www.opengl-tutorial.org/beginners-tutorials/tutorial-6-keyboard-and-mouse/ as a guide
*/
static void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	int height, width;
	float mouseSpeed = 1.0f;
	glfwGetWindowSize(window, &width, &height);

	double deltaTime = glfwGetTime();
	glfwSetTime(0);

	horizontalCam += (mouseSpeed * deltaTime * (width / 2 - xpos));
	verticalCam += (mouseSpeed * deltaTime * (height / 2 - ypos));

	glfwSetCursorPos(window, width/2, height/2);
	
}

/* Called whenever the window is resized. The new window size is given, in pixels. */
static void reshape(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	aspect_ratio = ((float)w / 640.f * 4.f) / ((float)h / 480.f * 3.f);
}

/* change view angle, exit upon ESC */
static void keyCallback(GLFWwindow* window, int key, int s, int action, int mods)
{
	/* Enable this call if you want to disable key responses to a held down key*/
	//if (action != GLFW_PRESS) return;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == 'W') //Move forward, apply direction looked at by camera as forward vector
	{
		cam_x += cam_x_mod;
		cam_z += cam_z_mod;
		cam_y += cam_y_mod;
	}
	if (key == 'S') //Move Backward, same as forward but opposite
	{
		cam_x -= cam_x_mod;
		cam_z -= cam_z_mod;
		cam_y -= cam_y_mod;
	}

	if (key == 'H' && action != GLFW_PRESS) //Increase Perlin Height modifier
	{
		heightmod++;
		if (heightmod > 30)
		{
			heightmod = 10;
		}
	}

	if (key == 'M' && action != GLFW_PRESS)
	{
		colourmode = !colourmode;
		cout << "colourmode=" << colourmode << endl;
	}

	/* Cycle between drawing vertices, mesh and filled polygons */
	if (key == 'N' && action != GLFW_PRESS)
	{
		drawmode++;
		if (drawmode > 2) drawmode = 0;
	}

}

/* Entry point of program */
int main(int argc, char* argv[])
{
	cout << "[Program Starting!]" << endl;
	GLWrapper* glw = new GLWrapper(1024, 768, "BlockWorld");
	cout << "[GLFW success!]" << endl;

	glw->setMouseCallback(mouseCallback);
	glw->setRenderer(display);
	glw->setKeyCallback(keyCallback);
	glw->setReshapeCallback(reshape);

	cout << "[Passed 0.0]" << endl;

	init(glw);

	cout << "[Passed 1]" << endl;

	glw->eventLoop();

	cout << "[Passed 2]" << endl;

	delete(glw);
	return 0;
}


