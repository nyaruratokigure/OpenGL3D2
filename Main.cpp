#include <iostream>
#include "Src/TitleScene.h"

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
	SceneStack& sceneStack = SceneStack::Instance();
	sceneStack.Push(std::make_shared<TitleScene>());

	for (;;) {
		const float deltaTime = 1.0f / 60.0f;
		sceneStack.Update(deltaTime);
		sceneStack.Render();
	}
}


