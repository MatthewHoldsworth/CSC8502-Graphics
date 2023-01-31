#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Light.h"
#include <string>
#include <vector>

class HeightMap;
class Camera;

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer(void);

	void RenderScene() override;
	void UpdateScene(float dt) override;

	//methods for managing camera instructions
	void passInstruction(float, float, Vector3);
	void clearCamera();
	bool cameraBusy();
protected:
	void DrawHeightMap(SceneNode*);
	void DrawWater();
	void DrawSkybox();
	//void DrawShadowScene();
	void SetLights(Shader*);
	void DrawNode(SceneNode*);

	SceneNode* root;
	HeightMap* heightMap;
	Camera* camera;
	vector<GLuint> textures;
	vector<GLuint> bumpmaps;

	Light light[2];
	GLuint lightBuffer[2];

	//GLuint shadowTex;
	//GLuint shadowFBO;
	//GLuint sceneDiffuse;
	//GLuint sceneBump;

	vector<Mesh*> sceneMeshes;

	Shader* reflectShader;
	Shader * skyboxShader;
	//Shader* shadowShader;

	Mesh* meshes[2];

	GLuint cubeMap;
	GLuint waterTex;

	float waterRotate;
	float waterCycle;
	float sceneTime;
};