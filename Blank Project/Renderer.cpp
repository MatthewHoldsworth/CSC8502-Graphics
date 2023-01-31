#include "Renderer.h"
#include "../nclgl/camera.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/CubeRobot.h"

#define SHADOWSIZE 2048

enum Meshes
{
	Quad,
	Cube
};

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	heightMap = new HeightMap(TEXTUREDIR"HeightMap.png");
	
	meshes[0] = Mesh::GenerateQuad();
	meshes[1] = Mesh::LoadFromMeshFile("OffsetCubeY.msh");

	camera = new Camera(0, 225, Vector3());
	Vector3 dimensions = heightMap->GetHeightmapSize();
	camera->SetPosition(dimensions  * Vector3(-0.5, 2, -0.5));
	
	light[0] = Light(dimensions * Vector3(0 * 0.5, 1.5f, 0 * 0.5f), Vector4(1, 1, 1, 1), dimensions.x * 0.5f);
	light[1] = Light(dimensions * Vector3(0.5, 0.3f,0.5f), Vector4(1, 0.6, 0, 1), dimensions.x * 0.05f);

	#pragma region  textureLoading
	textures.push_back(SOIL_load_OGL_texture(TEXTUREDIR"Sand.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	bumpmaps.push_back(SOIL_load_OGL_texture(TEXTUREDIR"SandBump.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	textures.push_back(SOIL_load_OGL_texture(TEXTUREDIR"Grass.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	bumpmaps.push_back(SOIL_load_OGL_texture(TEXTUREDIR"GrassBump.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	textures.push_back(SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	bumpmaps.push_back(SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	textures.push_back(SOIL_load_OGL_texture(TEXTUREDIR"Snow.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	bumpmaps.push_back(SOIL_load_OGL_texture(TEXTUREDIR"SnowBump.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	textures.push_back(SOIL_load_OGL_texture(TEXTUREDIR"doge.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	textures.push_back(SOIL_load_OGL_texture(TEXTUREDIR"fire.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	waterTex = SOIL_load_OGL_texture(TEXTUREDIR"water.TGA", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	cubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR"rusted_west.jpg", TEXTUREDIR"rusted_east.jpg",
		TEXTUREDIR"rusted_up.jpg", TEXTUREDIR"rusted_down.jpg",
		TEXTUREDIR"rusted_south.jpg", TEXTUREDIR"rusted_north.jpg",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	#pragma endregion

	#pragma region childInstantiations
	root = new SceneNode();
	root->SetMesh(heightMap);
	root->SetShader(new Shader("RootVertex.glsl", "RootFragment.glsl"));

	SceneNode* child0 = new SceneNode();
	child0->SetMesh(meshes[Cube]);
	child0->SetTexture(textures[textures.size()-1]);
	child0->SetModelScale(Vector3 (25,25,25));
	child0->SetTransform( Matrix4::Translation( dimensions * Vector3(0.5, 0.15, 0.5)));
	child0->SetShader(new Shader("TexturedVertex.glsl", "TexturedFragment.glsl"));

	SceneNode* child1 = new SceneNode(); //empty node for rotating child around axis
	SceneNode* child2 = new SceneNode();
	child2->SetMesh(meshes[Cube]);
	child2->SetTexture(textures[textures.size() - 2]);
	child2->SetModelScale(Vector3(25, 25, 25));
	child2->SetTransform(Matrix4::Translation(Vector3(100,0, 100)));
	child2->SetShader(new Shader("TexturedVertex.glsl", "TexturedFragment.glsl"));

	child1->AddChild(child2);
	child0->AddChild(child1);
	root->AddChild(child0);
	#pragma endregion

	for (int i = 0; i < textures.size(); i++) {
		SetTextureRepeating(textures[i], true);
	}
	for (int i = 0; i < bumpmaps.size(); i++) {
		SetTextureRepeating(bumpmaps[i], true);
	}
	SetTextureRepeating(waterTex, true);

	reflectShader = new Shader("reflectVertex.glsl", "reflectFragment.glsl");
	skyboxShader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");
	//shadowShader = new Shader("shadowVert.glsl", "shadowFrag.glsl");

	if (!skyboxShader->LoadSuccess() ||
		!reflectShader->LoadSuccess()){
		return;
	}

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
		(float)width / (float)height, 45.0f);

	glGenBuffers(1, lightBuffer);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glEnable(GL_LINEAR_MIPMAP_NEAREST);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	waterRotate = 0.0f;
	waterCycle = 0.0f;
	sceneTime = 0.0f;
	init = true;
} 

Renderer ::~Renderer(void) {
	glDeleteBuffers(1, lightBuffer);

	for (auto& i : meshes)
		delete i;
	

	delete heightMap;
	delete camera;
	delete root;
	delete reflectShader;
	delete skyboxShader;
}

void Renderer::UpdateScene(float dt) {
	sceneTime += dt * 10;

	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
	root->Update(dt);
	waterRotate += dt * 1.0f;
	waterCycle += dt * 0.15f; 
	root->GetChild(0)->GetChild(0)->SetTransform(Matrix4::Rotation(sceneTime,Vector3(0.0f,1.0f,0.0f)));
}

void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DrawSkybox();
	//DrawShadowScene();
	DrawHeightMap(root);
	DrawNode(root);
	DrawWater();
}

void Renderer::SetLights(Shader* s) {
	glBindBuffer(GL_UNIFORM_BUFFER, *lightBuffer);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(Light), light, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, *lightBuffer);
	int ubo = glGetUniformBlockIndex(s->GetProgram(), "Lights");
	glUniformBlockBinding(s->GetProgram(), ubo, 3);
	
	glBindBufferRange(GL_UNIFORM_BUFFER, 3, *lightBuffer, 0, 3);

}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);
	
	BindShader(skyboxShader);
	UpdateShaderMatrices();
	
	meshes[Quad]->Draw();
	
	glDepthMask(GL_TRUE);
}

void Renderer::DrawNode(SceneNode* n) {
	if (n != root && n->GetMesh()) {
		BindShader(n->GetShader());
		UpdateShaderMatrices();

		Matrix4 model = n->GetWorldTransform() *
			Matrix4::Scale(n->GetModelScale());

		glUniformMatrix4fv(
			glGetUniformLocation(n->GetShader()->GetProgram(),
				"modelMatrix"), 1, false, model.values);

		glUniform1i(glGetUniformLocation(
			n->GetShader()->GetProgram(), "diffuseTex"), 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, n->GetTexture());

		n->Draw(*this);

	}

	for (vector <SceneNode*>::const_iterator i = n->GetChildIteratorStart();
		i != n->GetChildIteratorEnd(); ++i) {
		DrawNode(*i);
	}
}

void Renderer::DrawHeightMap(SceneNode* n) {
	if (n->GetMesh()) {
		
		BindShader(n->GetShader());

		#pragma region textureBinding
		glUniform1i(glGetUniformLocation(
			n->GetShader()->GetProgram(), "diffuseTex[0]"), 0);
		glUniform1i(glGetUniformLocation(
			n->GetShader()->GetProgram(), "diffuseTex[1]"), 1);
		glUniform1i(glGetUniformLocation(
			n->GetShader()->GetProgram(), "diffuseTex[2]"), 2);
		glUniform1i(glGetUniformLocation(
			n->GetShader()->GetProgram(), "diffuseTex[3]"), 3);

		glUniform1i(glGetUniformLocation(
			n->GetShader()->GetProgram(), "bumpTex[0]"), 4);
		glUniform1i(glGetUniformLocation(
			n->GetShader()->GetProgram(), "bumpTex[1]"), 5);
		glUniform1i(glGetUniformLocation(
			n->GetShader()->GetProgram(), "bumpTex[2]"), 6);
		glUniform1i(glGetUniformLocation(
			n->GetShader()->GetProgram(), "bumpTex[3]"), 7);
		

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textures[1]);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, textures[2]);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, textures[3]);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, bumpmaps[0]);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, bumpmaps[1]);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, bumpmaps[2]);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, bumpmaps[3]);
		#pragma endregion

		modelMatrix.ToIdentity();
		textureMatrix.ToIdentity(); 

		glUniform3fv(glGetUniformLocation(n->GetShader()->GetProgram(),
			"cameraPos"), 1, (float*)&camera->GetPosition());

		SetLights(n->GetShader());
		glUniform1f(glGetUniformLocation(n->GetShader()->GetProgram(),
			"terrainHeight"), heightMap->GetHeightmapSize().y);
		glUniform1f(glGetUniformLocation(n->GetShader()->GetProgram(),
			"time"), sceneTime);

		UpdateShaderMatrices();
		
		heightMap->Draw();
	}

	for (vector <SceneNode*>::const_iterator i = n->GetChildIteratorStart();
		i != n->GetChildIteratorEnd(); ++i) {
		DrawNode(*i);
	}

}

void Renderer::DrawWater() {
	BindShader(reflectShader);
	
	glUniform3fv(glGetUniformLocation(reflectShader->GetProgram(),
		"cameraPos"), 1, (float*)&camera->GetPosition());
	
	glUniform1i(glGetUniformLocation(
		reflectShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(
		reflectShader->GetProgram(), "cubeTex"), 2);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterTex);
	
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
	Vector3 hSize = heightMap->GetHeightmapSize();
	
	modelMatrix =
		Matrix4::Translation(Vector3(hSize.x * 0.5f,hSize.y*0.08,hSize.z*0.5)) *
		Matrix4::Scale(hSize * 0.5f) *
		Matrix4::Rotation(270, Vector3(1, 0, 0));
	
	textureMatrix =
		Matrix4::Translation(Vector3(waterCycle, 0.0f, waterCycle)) *
		Matrix4::Scale(Vector3(10, 10, 10)) *
		Matrix4::Rotation(waterRotate, Vector3(0, 0, 1));
	
	UpdateShaderMatrices();
	meshes[Quad]->Draw();
}

void Renderer::passInstruction(float pitch, float yaw, Vector3 position) {
	camera->addInstruction(yaw, pitch, position);
}
void Renderer::clearCamera() {
	camera->clearInstructions();
}
bool Renderer::cameraBusy() {
	return camera->instructionExist();
}

/*
void Renderer::DrawShadowScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	BindShader(shadowShader);

	viewMatrix = Matrix4::BuildViewMatrix(
	light->GetPosition(), Vector3(0, 0, 0));
	projMatrix = Matrix4::Perspective(1, 100, 1, 45);
	shadowMatrix = projMatrix * viewMatrix; //used later

	for (int i = 0; i < 4; ++i) {
		modelMatrix = sceneTransforms[i];
		UpdateShaderMatrices();
		sceneMeshes[i]->Draw();
	}

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
*/