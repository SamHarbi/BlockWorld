#version 300 es
// Fragment shader that implements Oren Nayar (diffuse)
// per-fragment lighting.
// Modified Version from fraglight.zip uploaded to the lightning section of mydundee
// Used by Trees
// Sameer Al Harbi 2022

// Global constants (for this vertex shader)
mediump vec4 global_ambient = vec4(0.05, 0.05, 0.05, 1.0);
int  shininess = 8;

// Inputs from the vertex shader
in mediump vec3 fnormal, flightdir, fposition;
in mediump vec4 fdiffusecolour;
in mediump vec2 ftexcoord;

uniform sampler2D tex1;	

uniform int attenuationmode;
uniform int emitmode;

const mediump float PI = 3.141592653;
const mediump float roughness = 0.99;


// Output pixel fragment colour
out mediump vec4 outputColor;
void main()
{
	// Create a vec4(0, 0, 0) for our emmissive light but set to zero unless the emitmode flag is set
	mediump vec4 emissive = vec4(0);				
	mediump vec4 fambientcolour = fdiffusecolour * 0.1;
	
	// Normalise our input vectors, these may be the same for other BRDFs
	mediump vec3 normal = normalize(fnormal);
    mediump vec3 view_dir = normalize(-fposition.xyz);
    mediump vec3 light_dir = normalize(flightdir);
	mediump float distancetolight = length(flightdir);
	
	// Calculate these dot products which are used in the Oren Nayar equations below
    mediump float NdotL = dot(normal, light_dir);
    mediump float NdotV = dot(normal, view_dir); 

    mediump float angleVN = acos(NdotV);
    mediump float angleLN = acos(NdotL);

	// Implement the Oren Nayar equations
    mediump float alpha = max(angleVN, angleLN);
    mediump float beta = min(angleVN, angleLN);
    mediump float gamma = dot(view_dir - normal * dot(view_dir, normal), light_dir - normal * dot(light_dir, normal));

    mediump float roughnessSquared = roughness * roughness;
    mediump float roughnessSquared9 = (roughnessSquared / (roughnessSquared + 0.09));

    // calculate C1, C2 and C3
    mediump float C1 = 1.0 - 0.5 * (roughnessSquared / (roughnessSquared + 0.33));
    mediump float C2 = 0.45 * roughnessSquared9;

    if(gamma >= 0.0) {
        C2 *= sin(alpha);
    } else {
        C2 *= (sin(alpha) - pow((2.0 * beta) / PI, 3.0));
    }

    mediump float powValue = (4.0 * alpha * beta) / (PI * PI);
    mediump float C3  = 0.125 * roughnessSquared9 * powValue * powValue;

    // now calculate both main parts of the formula
    mediump float A = gamma * C2 * tan(beta);
    mediump float B = (1.0 - abs(gamma)) * C3 * tan((alpha + beta) / 2.0);

    // put it all together
    mediump float L1 = max(0.0, NdotL) * (C1 + A + B);

    // also calculate interreflection
    mediump float twoBetaPi = 2.0 * beta / PI;

    mediump float L2 = 0.17 * max(0.0, NdotL) * (roughnessSquared / (roughnessSquared + 0.13)) * (1.0 - gamma * twoBetaPi * twoBetaPi);

    mediump vec4 oren_nayar = vec4(fdiffusecolour.xyz * (L1 + L2), 1.0);
	
	// Calculate the attenuation factor;
	// Turn off attenuation if attenuationmode is not set to 1 (on)
	mediump float attenuation;
	if (attenuationmode != 1)
	{
		attenuation = 1.0;
	}
	else
	{
		// Define attenuation constants. These could be uniforms for greater flexibility
		mediump float attenuation_k1 = 0.5;
		mediump float attenuation_k2 = 0.5;
		mediump float attenuation_k3 = 0.5;
		attenuation = 1.0 / (attenuation_k1 + attenuation_k2*distancetolight + 
								   attenuation_k3 * pow(distancetolight, 2.0));
	}
	
	// If emitmode is 1 then we enable emmissive lighting
	if (emitmode == 1) emissive = vec4(1.0, 1.0, 0.8, 1.0); 

	// Extract the texture colour to colour our pixel
	mediump vec4 texcolour = texture(tex1, ftexcoord);

	// Calculate the output colour, includung attenuation on the diffuse and specular components
	// Note that you may want to exclude the ambient from the attenuation factor so objects
	// are always visible, or include a global ambient
	outputColor = (attenuation*(fambientcolour + oren_nayar) + emissive + global_ambient) * texcolour;

}