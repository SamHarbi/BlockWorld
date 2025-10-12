#pragma once

#include "ChunkBlock.h"
#include "cube_tex.h"
#include "ModelLoader/tiny_loader_texture.h"
#include <vector>


class BlockWorld {
public:
    BlockWorld();

    const static int numOfPrograms = 3; //How many programs will be used
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
    Cube cube;

    ChunkBlock chunkblock; //Single 16x16x16 Chunk Block
    glm::vec3 megaChunk[9]; //Positions of all visible Chunks around a player
    glm::vec3 chunkOrigin; //Origin Point of first chunk where player starts

    // Define the normal matrix used by Trees lightning
    glm::mat3 normalmatrix;


};