#version 400 core
uniform sampler2D diffuseTex[3];
//A uniform declaration in any shader type
//uniform samplerBuffer aTBOSampler;

uniform float terrainHeight;

in Vertex {
vec2 texCoord;
vec3 position;
} IN;

out vec4 fragColour;

void main(void) {
//in the main function somewhere ...
//fragColour = texelFetch(aTBOSampler, ivec2(IN.texCoord),0).rgba;
//gvec4 moreData :: texelFetch(aTBOSampler ,0). rgb;


float percent = IN.position.y/terrainHeight;

//float weight[3];
//weight[0] = 0.1;
//weight[1] = 0.2;
//weight[2] = 1.0;

//int type;

//for(int i = 0; i< weight.length ; i++)
//{
	//if(percent < weight[i]){
		//if(i==0){
			//fragColour = texture(diffuseTex[i]  , IN.texCoord );
		//}
		//else
		//{
			//fragColour = mix(texture(diffuseTex[i]  , IN.texCoord ), texture(diffuseTex[i-1] , IN.texCoord ), percent);
		//}
	//}
//};

//fragColour = texture(diffuseTex[1]  , IN.texCoord );

if(percent > 0.1){
fragColour = mix(texture(diffuseTex[0]  , IN.texCoord ), texture(diffuseTex[1] , IN.texCoord ), percent);
}
else{
fragColour = texture(diffuseTex[0], IN.texCoord ), texture(diffuseTex[1] , IN.texCoord );
}
}
