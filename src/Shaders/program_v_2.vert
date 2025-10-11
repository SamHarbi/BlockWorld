// Veretx shader to implement Phong shading (per fragment lighting)
// Modified Version from fraglight.zip uploaded to the lightning section of mydundee
// Used by Trees
// Sameer Al Harbi 2022

#version 420

// These are the vertex attributes
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

// Uniform variables are passed in from the application
uniform mat4 model, view, projection, light_view;
uniform mat3 normalmatrix;
uniform uint colourmode;

// Outputs to send to the fragment shader
out vec3 fnormal;
out vec3 flightdir, fposition;
out vec4 fdiffusecolour, fambientcolour;

// Output the  texture coordinate - just pass it through
out vec2 ftexcoord;

vec3 light_dir = vec3(0.0, 0.0, 10.0);

void main()
{
	vec4 position_h = vec4(position, 1.0);
	vec3 light_pos3 = light_dir.xyz;	

	// Switch the vertex colour based on the colourmode
	if (colourmode == 1)
		fdiffusecolour = vec4(0, 0.2, 0, 1.0);
	else
		fdiffusecolour = vec4(1.0, 1.0, 1.0, 1.0);

	// Define our vectors for calculating diffuse and specular lighting
	mat4 mv_matrix = light_view * model;				// Calculate the model-view transformation
	fposition = (mv_matrix * position_h).xyz;	// Transform the vertex position (x, y, z) into eye-space
	fnormal = normalize(normalmatrix * normal);	// Modify the normal by the normal-matrix (i.e. transform to eye-space )
	flightdir = light_pos3 - fposition;			// Calculate the vector from the light position to the vertex in eye space

	// Define the vertex position
	gl_Position = projection * view * model * position_h;

	// Pas through the texture coordinate
	ftexcoord = texcoord;
}

