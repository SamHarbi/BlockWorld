#version 300 es
// Based on Lab5 Solution
// Fragment shader which combines colour and texture using a samplerCube
// The colour is the lighting colour outputted by the vertex shader
// Iain Martin 2019 - Edited by Sameer Al Harbi 2022
// Used by Terrain 

in mediump vec4 fcolour;
out mediump vec4 outputColor;
in mediump vec3 ftexcoord;
in mediump vec4 fposition;

// Fog parameters, could make them uniforms and pass them into the fragment shader
mediump float fog_maxdist = 32.0;
mediump float fog_mindist = 6.0;
mediump vec4 fog_colour = vec4(0.4, 0.4, 0.4, 1.0);

uniform samplerCube tex1;	

void main()
{
	// Calculate fog
	mediump float dist = length(fposition.xyz);
	mediump float fog_factor = (fog_maxdist - dist) /
                  (fog_maxdist - fog_mindist);
	fog_factor = clamp(fog_factor, 0.0, 1.0);
	
	// Extract the texture colour to colour our pixel
	mediump vec4 texcolour = texture(tex1, ftexcoord);

	// Set the poixel colour to be a combination of our lit colour and the texture
	outputColor = mix(fog_colour ,fcolour * texcolour, fog_factor);

}