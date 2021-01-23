/*
@file GLFWEW.h
*/
#ifndef GLFWEW_H_INCLUDED
#define GLFWEW_H_INCLUDED
#include <GL/glew.h>
#include "GamePad.h"
#include<GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

namespace GLFWEW {

/*
GLFWとGLEWのラッパークラス
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
	double Time() const;
	int Width() const { return width; }
	int Height() const { return height; }
	bool IsKeyPressed(int key)const;
	const GamePad& GetGamePad() const;
	void InputMouse();


private:
	Window();
	~Window();
	Window(const Window&) = delete;
	Window&operator=(const Window&) = delete;
	void UpdateGamePad();

	bool isGLFWInitialized = false;
	bool isInitialized = false;
	GLFWwindow* window = nullptr;
	int width = 0;
	int height = 0;
	double prevTime = 0;
	double deltaTime = 0;
	GamePad gamepad;
	
	// マウスカーソルの座標(画面中心を0とするOpenGLの二次元ワールド座標系).
	glm::vec2 cursorPosition = glm::vec2(0);
};

}//namespase GLFWEW

#endif //GLFWEW_H_INCLUDED

