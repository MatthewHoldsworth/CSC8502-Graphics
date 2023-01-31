#version 330 core
uniform sampler2D diffuseTex;

in Vertex {
vec2 texCoord;
} IN;

out vec4 fragColour;
void main(void) {
if(texture(diffuseTex , IN.texCoord ).a > 0.5){
fragColour = texture(diffuseTex , IN.texCoord );
//fragColour = vec4(0,0.5,0,1);
}
}
