#version 330 core

uniform sampler2D diffuseTex[4];
uniform sampler2D bumpTex[4];
uniform vec3 cameraPos;
uniform float terrainHeight;
uniform float time;

struct Light{
	vec4 lightColour;
	vec3 lightPos;
	float lightRadius;
};

layout (std140) uniform Lights{
	Light lights[2];
};

in Vertex {
	vec3 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent; 
	vec3 binormal; 
	vec3 worldPos;
} IN;

out vec4 fragColour;

float fog(){
	float distance = length(cameraPos - IN.worldPos);
	float fPercent = clamp(distance/5000,0.0,1.0); 
	vec4 grey = vec4(0.33,0.33,0.34,1.0);

	fragColour.rgb = (fragColour.rgb * (1-fPercent)) + (grey.rgb * (fPercent));
	return fPercent;
}

void lighting(vec4 diffuse, vec3 bump){
	vec3 viewDir = normalize(cameraPos - IN.worldPos );

	for(int i = 0;i<lights.length;i++){
		vec3 incident;
		float attenuation;
		float distance = length(lights[i].lightPos - IN.worldPos );
		if(i ==0 ){
			incident = normalize(-IN.worldPos);
			attenuation = 1.0;
		}else{
			incident = normalize(lights[i].lightPos - IN.worldPos );
			attenuation = 1.0 - clamp(distance / lights[i].lightRadius , 0.0, 1.0);
		}
		//vec3 viewDir = normalize(cameraPos - IN.worldPos );
		vec3 halfDir = normalize(incident + viewDir );

		float lambert = max(dot(incident , bump), 0.0f);

		//float attenuation = 1.0/(distance*distance);
		float specFactor = clamp(dot(halfDir , bump ) ,0.0 ,1.0);
		specFactor = pow(specFactor , 60.0 );
		vec3 surface = (diffuse.rgb * (lights[i].lightColour.rgb * lights[i].lightColour.w));
		fragColour.rgb += surface * lambert * attenuation;
		fragColour.rgb += ((lights[i].lightColour.rgb  * lights[i].lightColour.w) * specFactor )* attenuation *0.33;
		fragColour.rgb += surface * 0.1f; // ambient!
		fragColour.a = diffuse.a;
	}
}

void main(void) {

	vec4 diffuse;
	vec3 bump;

	float percent = clamp(IN.worldPos.y/terrainHeight, 0.0, 1.0);
	
	mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal ));

	float weight[4];
	weight[0] = 0.1;
	weight[1] = 0.15;
	weight[2] = 0.4;
	weight[3] = 0.7;

	for(int i = 0; i< diffuseTex.length; i++){
		if(percent < weight[i] || i==diffuseTex.length-1){
			if(i == 0 || percent >= weight[i]){
				diffuse = texture(diffuseTex[i]  , IN.texCoord );
				bump = texture(bumpTex[i] , IN.texCoord ).rgb;
			}else{
				diffuse = mix(texture(diffuseTex[i-1]  , IN.texCoord ), texture(diffuseTex[i] , IN.texCoord ), (percent-weight[i-1])/(weight[i]-weight[i-1]));
				bump = mix(texture(bumpTex[i-1] , IN.texCoord ).rgb, texture(bumpTex[i] , IN.texCoord ).rgb, (percent-weight[i-1])/(weight[i]-weight[i-1]));
			}
			//bump = texture(bumpTex[i] , IN.texCoord ).rgb;
			bump = normalize(TBN * normalize(bump * 2.0 - 1.0));
			i = diffuseTex.length;
		}
	}

	
	lighting(diffuse, bump);
	
	fog();
	
}
