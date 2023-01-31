#include "Renderer.h"
#include "../nclgl/camera.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/CubeRobot.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	heightMap = new HeightMap(TEXTUREDIR"HeightMap.png");
	camera = new Camera(-40, 270, Vector3());
	Vector3 dimensions = heightMap->GetHeightmapSize();
	camera->SetPosition(dimensions  * Vector3(0.5, 2, 0.5));

	for (int i = 0; i < 3; i++) {
		light[i] = Light(dimensions * Vector3(i * 0.5, 1.5f, i * 0.5f), Vector4(1, 1, 1, 1), dimensions.x * 0.5f);
	}

	textures.push_back(SOIL_load_OGL_texture(TEXTUREDIR"Grass.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	textures.push_back(SOIL_load_OGL_texture(TEXTUREDIR"Snow.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	root = new SceneNode();
	root->SetMesh(heightMap);
	root->SetTexture(textures[0]);
	root->SetShader(new Shader("RootVertex.glsl", "RootFragment.glsl"));

	for(int i=0;i<textures.size();i++) SetTextureRepeating(textures[i], true);

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
		(float)width / (float)height, 45.0f);

	glGenBuffers(1, &textureBuffer);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	init = true;
} 

Renderer ::~Renderer(void) {
	delete heightMap;
	delete camera;
	delete root;
	//delete shader;ou
}

void Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
	root->Update(dt);
}

void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DrawNode(root);
}

void Renderer::DrawNode(SceneNode* n) {
	if (n->GetMesh()) {
		
		BindShader(n->GetShader());
		UpdateShaderMatrices();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_BUFFER, textures[0]);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, textureBuffer);

		/*
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, textures[0]);
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, textures[1]);
		*/

		glUniform1f(glGetUniformLocation(n->GetShader()->GetProgram(),
			"terrainHeight"), heightMap->GetHeightmapSize().y);
		heightMap->Draw();
	}

	for (vector <SceneNode*>::const_iterator i = n->GetChildIteratorStart();
		i != n->GetChildIteratorEnd(); ++i) {
		DrawNode(*i);
	}

}