#include "Renderer.h"
#include "../nclgl/Light.h"
#include "../nclgl/Camera.h"
#include "../nclgl/HeightMap.h"

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	heightMap = new HeightMap(TEXTUREDIR"noise.png");
	texture = SOIL_load_OGL_texture(
			TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	
	shader = new Shader("PerPixelVertex.glsl", "PerPixelFragmentW.glsl");
	
	if (!shader->LoadSuccess() || !texture) {
		return;
	}
	SetTextureRepeating(texture, true);
	Vector3 heightmapSize = heightMap->GetHeightmapSize();

	camera = new Camera(-45.0f, 0.0f,
		heightmapSize * Vector3(0.5f, 5.0f, 0.5f));

	for (int i = 0; i < 3; i++) {
		light[i] = Light(heightmapSize * Vector3(i * 0.5, 1.5f, i * 0.5f), Vector4(1, 1, 1, 1), heightmapSize.x * 0.5f);
	}
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
			(float)width / (float)height, 45.0f);

	glGenBuffers(1, &lightBuffer);
	glEnable(GL_DEPTH_TEST);
	init = true;
}

Renderer ::~Renderer(void) {
	delete camera;
	delete heightMap;
	delete shader;
}

void Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	BindShader(shader);


	glUniform1i(glGetUniformLocation(
		shader->GetProgram(), "diffuseTex"), 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glUniform3fv(glGetUniformLocation(shader->GetProgram(),
		"cameraPos"), 2, (float*)&camera->GetPosition());

	glBindBuffer(GL_UNIFORM_BUFFER, lightBuffer);
	glBufferData(GL_UNIFORM_BUFFER, 3 * sizeof(Light), light, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, lightBuffer);
	int ubo = glGetUniformBlockIndex(shader->GetProgram(), "Lights");
	glUniformBlockBinding(shader->GetProgram(), ubo, 1);

	glBindBufferRange(GL_UNIFORM_BUFFER, 1, lightBuffer, 0, 3);

	UpdateShaderMatrices();

	heightMap->Draw();
}