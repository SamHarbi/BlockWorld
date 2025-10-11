// Based on Lab5 Solution
// Fragment shader which combines colour and texture using a samplerCube
// The colour is the lighting colour outputted by the vertex shader
// Iain Martin 2019 - Edited by Sameer Al Harbi 2022
// Used by Terrain 

#version 300 es

in vec4 fcolour;
out vec4 outputColor;
in vec3 ftexcoord;

uniform samplerCube tex1;	

void main()
{
	// Extract the texture colour to colour our pixel
	vec4 texcolour = texture(tex1, ftexcoord);

	// Set the poixel colour to be a combination of our lit colour and the texture
	outputColor = fcolour * texcolour;

}