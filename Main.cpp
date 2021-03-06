#include <iostream>
#include "Src/TitleScene.h"
#include <Windows.h>
#include "Src/GLFWEW.h"
#include "Src/SkeletalMesh.h"
#include "Src/Audio/Audio.h"
#include "Src/EventScript.h"

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

	//音声再生プログラムを初期化する
	Audio::Engine& audioEngine = Audio::Engine::Instance();
	if (!audioEngine.Initialize()) {
		return 1;
	}

	//スケルタル・アニメーションを利用可能にする
	Mesh::SkeletalAnimation::Initialize();

	SceneStack& sceneStack = SceneStack::Instance();
	sceneStack.Push(std::make_shared<TitleScene>());

	EventScriptEngine& scriptEngine = EventScriptEngine::Instance();
	scriptEngine.Init();


	window.InitTimer();
	while (!window.ShouldClose()) {

		// ESCキーが押されたら終了ウィンドウを表示
		if (window.IsKeyPressed(GLFW_KEY_ESCAPE)) {
			if (MessageBox(nullptr, L"ゲームを終了しますか？", L"終了", MB_OKCANCEL) == IDOK) {
				break;
			}
		}

		//スケルタル・アニメーション用データの作成準備
		Mesh::SkeletalAnimation::ResetUniformData();
		
		const float deltaTime = window.DeltaTime();
		window.UpdateTimer();
		sceneStack.Update(deltaTime);
		scriptEngine.Update(deltaTime);

		//スケルタル・アニメーション用データをGPUメモリに転送
		Mesh::SkeletalAnimation::UploadUniformData();

		//音声再生プログラムを更新する
		audioEngine.Update();

		//バックバッファを消去する
		glClearColor(0.8f, 0.2f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//GLコンテキストのパラメータを設定
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);



		sceneStack.Render();
		scriptEngine.Draw();
		window.SwapBuffers();
	}
	//スケルタル・アニメーションの利用を終了する
	Mesh::SkeletalAnimation::Finalize();

	//音声再生プログラムを終了する
	audioEngine.Finalize();
}


