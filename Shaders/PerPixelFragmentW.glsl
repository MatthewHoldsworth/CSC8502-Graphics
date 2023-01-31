#version 330 core

uniform sampler2D diffuseTex;
uniform vec3 cameraPos;
//uniform vec4 lightColour;
//uniform vec3 lightPos;
//uniform float lightRadius;

struct Light{
	vec4 lightColour;
	vec3 lightPos;
	float lightRadius;
};

layout (std140) uniform Lights{
	Light lights[3];
};

in Vertex {
	vec3 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 worldPos;
} IN;

out vec4 fragColour;

void main(void) {
//Light lighting[3];
//fragColour = lights[0].lightColour;
for(int i = 0;i<3;i++){
	vec3 incident = normalize(lights[i].lightPos - IN.worldPos );
	vec3 viewDir = normalize(cameraPos - IN.worldPos );
	vec3 halfDir = normalize(incident + viewDir );

	vec4 diffuse = texture(diffuseTex , IN.texCoord );
	float lambert = max(dot(incident , IN.normal), 0.0f);
	float distance = length(lights[i].lightPos - IN.worldPos );
	float attenuation = 1.0 - clamp(distance / lights[i].lightRadius , 0.0, 1.0);
	//float attenuation = 1.0/(distance*distance);
	float specFactor = clamp(dot(halfDir , IN.normal ) ,0.0 ,1.0);
	specFactor = pow(specFactor , 60.0 );
	vec3 surface = (diffuse.rgb * (lights[i].lightColour.rgb * lights[i].lightColour.w));
	fragColour.rgb += surface * lambert * attenuation;
	fragColour.rgb += ((lights[i].lightColour.rgb  * lights[i].lightColour.w) * specFactor )* attenuation *0.33;
	fragColour.rgb += surface * 0.1f; // ambient!
	fragColour.a = diffuse.a;
	}
}