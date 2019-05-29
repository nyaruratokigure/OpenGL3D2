#include <iostream>
#include "Src/TitleScene.h"
#include "Src/GLFWEW.h"

int stack[100];
int data_size;

void push(int data)
{
	//スタックの一番上にデータを積む
	stack[data_size] = data;
	//データ数を１増やす
	++data_size;
}


int pop()
{
	int data = stack[data_size - 1];
	//データ数を１減らす
	--data_size;
	//取り出したデータを戻り値にする
	return data;
}

int main()
{
	GLFWEW::Window& window = GLFWEW::Window::Instance();
	if (!window.Init(1280, 720, "OpenGL 3D 2019")) {
		return 1;
	}

	SceneStack& sceneStack = SceneStack::Instance();
	sceneStack.Push(std::make_shared<TitleScene>());


	window.InitTimer();
	while (!window.ShouldClose()) {
		const float deltaTime = window.DeltaTime();
		window.UpdateTimer();
		sceneStack.Update(deltaTime);
		sceneStack.Render();
		window.SwapBuffers();
	}
}


