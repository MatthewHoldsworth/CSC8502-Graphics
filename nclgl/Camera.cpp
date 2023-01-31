#include "Camera.h"
#include "Window.h"

void Camera::UpdateCamera(float dt) {
	if (instruction.size()==0) {
		transform.pitch -= (Window::GetMouse()->GetRelativePosition().y);
		transform.yaw -= (Window::GetMouse()->GetRelativePosition().x);

		transform.pitch = std::min(transform.pitch, 90.0f);
		transform.pitch = std::max(transform.pitch, -90.0f);

		if (transform.yaw < 0) {
			transform.yaw += 360.0f;

		}
		if (transform.yaw > 360.0f) {
			transform.yaw -= 360.0f;

		}
		Matrix4 rotation = Matrix4::Rotation(transform.yaw, Vector3(0, 1, 0));

		Vector3 forward = rotation * Vector3(0, 0, -1);
		Vector3 right = rotation * Vector3(1, 0, 0);

		float speed = 600.0f * dt;

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
			transform.position += forward * speed;

		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
			transform.position -= forward * speed;

		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
			transform.position -= right * speed;

		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
			transform.position += right * speed;

		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) {
			transform.position.y += speed;

		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) {
			transform.position.y -= speed;

		}
	}
	else {
		lerpInstruction(dt);
	}
}

Matrix4 Camera::BuildViewMatrix() {
	return Matrix4::Rotation(-transform.pitch, Vector3(1, 0, 0)) *
		Matrix4::Rotation(-transform.yaw, Vector3(0, 1, 0)) *
		Matrix4::Translation(-transform.position);
};

void Camera::addInstruction(float yaw, float pitch, Vector3 location) {
	instruction.push_back(cameraTransform(yaw, pitch, location));

}


float lerp(float start, float end, float amnt) {
	return (start * (1-amnt)) + (end * amnt);
}

Vector3 lerpVec3(Vector3 start, Vector3 end, float amnt) {
	return (start * (1 - amnt)) + (end * amnt);
}

float maxFunc(float max, float value) {
	return value > max ? max : value;
}

float minFunc(float min, float value) {
	return value < min ? min : value;
}

float clamp(float min, float max, float value) {
	return minFunc(min, maxFunc(max, value));
}

void Camera::lerpInstruction(float dt) {
	transform.yaw = lerp(transform.yaw, instruction[0].yaw,  clamp(0.01f, 1.0f, autoSpeed*dt));
	transform.pitch = lerp(transform.pitch, instruction[0].pitch, clamp(0.01f, 1.0f, autoSpeed * dt));
	transform.position = lerpVec3(transform.position, instruction[0].position, clamp(0.01f, 1.0f, autoSpeed * dt));
	timer -= dt;
	if (timer <=0.0f) {
		instruction.erase(instruction.begin());
		timer = instructTimer;
	}
	
}
void Camera::clearInstructions() {
	instruction.clear();
}
bool Camera::instructionExist() {
	if (instruction.size()>0) {
		return true;
	}
	else {
		return false;
	}
}


