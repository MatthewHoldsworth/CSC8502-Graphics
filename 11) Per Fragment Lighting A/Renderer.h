#pragma once
#include "../nclgl/OGLRenderer.h"

class HeightMap;
class Camera;
class Light; // Predeclare our new class type ...
class Shader;

//struct LightStruct {
//	Vector4 lightColour;
//	Vector3 lightPos;
//	float lightRadius;
//};

class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
	~Renderer(void);
	
	void RenderScene() override;
	void UpdateScene(float dt) override;	
protected:
	HeightMap * heightMap;
	Shader * shader;
	Camera * camera;
	Light light[3]; //A new thing!
	GLuint texture;
	GLuint lightBuffer;
}; 
