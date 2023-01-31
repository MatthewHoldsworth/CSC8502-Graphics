#include "Renderer.h"
#include "../nclgl/camera.h"
#include "../nclgl/HeightMap.h"

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	heightMap = new HeightMap(TEXTUREDIR"HeightMap.png");
	camera = new Camera(-40, 270, Vector3());
	
	Vector3 dimensions = heightMap->GetHeightmapSize();
	camera->SetPosition(dimensions * Vector3(0.5, 2, 0.5));
	
	shader = new Shader("TerrainVertex.glsl",
			"TerrainFragment.glsl");
	if (!shader->LoadSuccess()) {
		return;
	}
	
	terrainTex = SOIL_load_OGL_texture(TEXTUREDIR"Grass.JPG",
			SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	terrainTex1 = SOIL_load_OGL_texture(TEXTUREDIR"Snow.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	//terrainTex2 = SOIL_load_OGL_texture(TEXTUREDIR"Snow.JPG",
		//SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	
	if (!terrainTex|| !terrainTex1 || !terrainTex2) {
		return;
	}
	
	SetTextureRepeating(terrainTex, true);
	SetTextureRepeating(terrainTex1, true);
	//SetTextureRepeating(terrainTex2, true);
	
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
			(float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	init = true;
}

Renderer ::~Renderer(void) {
	delete heightMap;
	delete camera;
	delete shader;	
}

void Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
}

void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	BindShader(shader);
	UpdateShaderMatrices();
	
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex[0]"), 0);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex[1]"), 1);

	/*
	glBindBuffer(GL_TEXTURE_BUFFER, terrainBuffer);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, terrainTex);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, terrainTex1);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, terrainBuffer);
	*/
	
	glActiveTexture(GL_TEXTURE0+0);
	glBindTexture(GL_TEXTURE_2D, terrainTex);
	glActiveTexture(GL_TEXTURE0+1);
	glBindTexture(GL_TEXTURE_2D, terrainTex1);

	glUniform1f(glGetUniformLocation(shader->GetProgram(),
		"terrainHeight"), heightMap->GetHeightmapSize().y);
	heightMap->Draw();
}