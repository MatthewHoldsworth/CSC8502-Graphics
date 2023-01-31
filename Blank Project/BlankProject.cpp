#include "../NCLGL/window.h"
#include "Renderer.h"

int main()	{
	Window w("Elden Ring 4: Every Elden has its Ring", 1280, 720, false);

	if(!w.HasInitialised()) {
		return -1;
	}
	
	Renderer renderer(w);
	if(!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	renderer.passInstruction(0, 200, Vector3(0, 50, 0));
	renderer.passInstruction(-40, 270, Vector3(1000, 1000, 1000));
	renderer.passInstruction(-40, 270, Vector3(3000, 300, 3000));
	renderer.passInstruction(0, 270, Vector3(3500, 300, 3500));
	renderer.passInstruction(-25, 200, Vector3(3500, 300, 3500));
	renderer.passInstruction(-25, 100, Vector3(3500, 300, 3500));
	renderer.passInstruction(5, 60, Vector3(3500, 300, 3500));
	renderer.passInstruction(-90, 270, Vector3(5000, 5000, 5000));

	bool loop = true;

	while(w.UpdateWindow()  && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		renderer.UpdateScene(w.GetTimer()->GetTimeDeltaSeconds());
		renderer.RenderScene();
		renderer.SwapBuffers();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
			Shader::ReloadAllShaders();
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_P)) {
			loop = true;
		}if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_O)) {
			renderer.clearCamera();
			loop = false;
		}if (loop && !renderer.cameraBusy()) { //statement for looping the instructions if not already in loop
			renderer.passInstruction(0, 200, Vector3(0, 50, 0));
			renderer.passInstruction(-40, 270, Vector3(1000, 1000, 1000));
			renderer.passInstruction(-40, 270, Vector3(3000, 300, 3000));
			renderer.passInstruction(0, 270, Vector3(3500, 300, 3500));
			renderer.passInstruction(-25, 200, Vector3(3500, 300, 3500));
			renderer.passInstruction(-25, 100, Vector3(3500, 300, 3500));
			renderer.passInstruction(5, 60, Vector3(3500, 300, 3500));
			renderer.passInstruction(-90, 270, Vector3(5000, 5000, 5000));
		}
	}
	return 0;
}