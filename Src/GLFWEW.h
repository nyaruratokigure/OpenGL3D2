/*
@file GLFWEW.h
*/
#ifndef GLFWEW_H_INCLUDED
#define GLFWEW_H_INCLUDED
#include <GL/glew.h>
#include "GamePad.h"
#include<GLFW/glfw3.h>

namespace GLFWEW {

/*
GLFW��GLEW�̃��b�p�[�N���X
*/
class Window {
public:
	static Window& Instance();
	bool Init(int w,int h, const char*title);
	bool ShouldClose()const;
	void SwapBuffers()const;

	void InitTimer();
	void UpdateTimer();
	double DeltaTime() const;
	bool IsKeyPressed(int key)const;
	const GamePad& GetGamePad() const;


private:
	Window();
	~Window();
	Window(const Window&) = delete;
	Window&operator=(const Window&) = delete;
	void UpdateGamePad();

	bool isGLFWInitialized = false;
	bool isInitialized = false;
	GLFWwindow* window = nullptr;
	double prevTime = 0;
	double deltaTime = 0;
	GamePad gamepad;
};

}//namespase GLFWEW

#endif //GLFWEW_H_INCLUDED
