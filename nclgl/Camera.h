#pragma once
#include "Matrix4.h"
#include "Vector3.h"
#include <vector>
#include <algorithm>

class Camera {
public:
	Camera(void) {
		this->transform.yaw = 0.0f;
		this->transform.pitch = 0.0f;
		this->timer = instructTimer;
		//this->lock = false;
	};

	Camera(float pitch, float yaw, Vector3 position) {
		this->transform.pitch = pitch;
		this->transform.yaw = yaw;
		this->transform.position = position;
		this->timer = instructTimer;
	}

	~Camera(void) {};

	void UpdateCamera(float);

	Matrix4 BuildViewMatrix();

	Vector3 GetPosition() const { return transform.position; }
	void SetPosition(Vector3 val) { transform.position = val; }

	float GetYaw() const { return transform.yaw; }
	void SetYaw(float y) { transform.yaw = y; }

	float GetPitch() const { return transform.pitch; }
	void SetPitch(float p) { transform.pitch = p; }

	void addInstruction(float , float , Vector3 );
	void clearInstructions();
	bool instructionExist();

protected:
	void lerpInstruction(float);

	struct cameraTransform
	{
		float yaw;
		float pitch;
		Vector3 position;
		cameraTransform() {
			yaw = 0;
			pitch = 0;
			position = Vector3(0,0,0);
		}

		cameraTransform(float y, float p, Vector3 pos) {
			yaw = y;
			pitch = p;
			position = pos;
		}
	};

	float autoSpeed = 0.0f;

	std::vector<cameraTransform> instruction;

	cameraTransform transform;
	float instructTimer =5.0f;
	float timer;
};
