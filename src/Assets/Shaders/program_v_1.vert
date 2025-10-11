#version 300 es
// Simple Vertex Skybox Shader
// Sameer Al Harbi 2022

// These are the vertex ins
in vec3 position;
in vec4 colour;
in vec3 normal;

// Uniform variables are passed in from the application
uniform mat4 model, view, projection;

// Output the  texture coordinate - just pass it through
out vec3 ftexcoord;


void main()
{
	// Define the vertex position
	gl_Position = projection * view * model * vec4(position.x, position.y, position.z, 1.0);

	// Pas through the texture coordinate
	ftexcoord = position;
}

