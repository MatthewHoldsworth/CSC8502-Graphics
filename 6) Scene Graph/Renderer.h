#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/window.h"
#include "../nclgl/Camera.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/CubeRobot.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer(void);

	void UpdateScene(float dt) override;
	void RenderScene() override;

protected:
	void DrawNode(SceneNode* n);

	vector<SceneNode*> root;
	Camera* camera;
	Mesh* cube;
	Shader* shader;
};