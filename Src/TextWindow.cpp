/*
@file TextWindow.cpp
*/
#include "TextWindow.h"
#include "GLFWEW.h"
#include <iostream>

/*
テキストウィンドウを初期化する

@param imagePath      ウィンドウの画像ファイル名
@param position       ウィンドウの表示位置(ドット)
@param textAreaMargin ウィンドウサイズとテキスト表示領域の間隔(ドット)
@param textAreaOffset テキスト表示領域の位置をずらす値(ドット)

@retval true  初期化成功
@retval false 初期化失敗
*/
bool TextWindow::Init(const char* imagePath, const glm::vec2& position,
	const glm::vec2& textAreaMargin, const glm::vec2& textAreaOffset)
{
	this->position = position;
	this->textAreaOffset = textAreaOffset;
	this->textAreaMargin = textAreaMargin;

	if (!spriteRenderer.Init(10, "Res/Sprite.vert", "Res/Sprite.frag")) {
		std::cerr << "[エラー]" << __func__ << ":テキストウィンドウの初期化に失敗\n";
		return false;
	}
	sprBackground = Sprite(Texture::Image2D::Create(imagePath));

	if (!fontRenderer.Init(2000)) {
		std::cerr << "[エラー]" << __func__ << ":テキストウィンドウの初期化に失敗\n";
		return false;
	}
	if (!fontRenderer.LoadFromFile("Res/font.fnt")) {
		std::cerr << "[エラー]" << __func__ << ":テキストウィンドウの初期化に失敗\n";
		return false;
	}
	return true;
}

/*
テキストウィンドウの状態を更新する

@param TextWindow::Update(float deltaTime)
*/
void TextWindow::Update(float deltaTime)
{
	//ウィンドウが閉じていたら更新しない
	if (!isOpen) {
		spriteRenderer.BeginUpdate();
		spriteRenderer.EndUpdate();
		fontRenderer.BeginUpdate();
		fontRenderer.EndUpdate();
		return;
	}
	//背景画像の更新
	sprBackground.Position(glm::vec3(position, 0));
	spriteRenderer.BeginUpdate();
	spriteRenderer.AddVertices(sprBackground);
	spriteRenderer.EndUpdate();

	//すべての文字が表示済み(outputCountがテキスト文字数以上)なら
	//フォントを更新する必要はない
	if (outputCount >= static_cast<int>(text.size())) {
		return;
	}
	//文字表示間隔が0より大きいなら、1文字ずつ表示する
	//0以下なら一気に表示する
	if (interval > 0) {
		outputTimer += deltaTime;
		const int n = static_cast<int>(outputTimer / interval);
		//表示する文字数が変化しないら更新不要
		if (n == 0) {
			return;
		}
		outputCount += n;
		if (outputCount >= static_cast<int>(text.size())) {
			outputCount = static_cast<int>(text.size());
		}
		outputTimer -= static_cast<float>(n)*interval;
	}
	else {
		outputCount = text.size();
	}

	//文章表示領域のサイズを計算
	const Texture::Image2DPtr tex = sprBackground.Texture();
	const glm::vec2 windowSize = glm::vec2(tex->Width(), tex->Height());
	const glm::vec2 textAreaSize = windowSize - textAreaMargin * 2.0f;

	//現在までに出力するべき文字を行単位で取得
	std::vector<std::wstring>rowList;
	float lineWidth = 0;//行の横幅
	int outputOffset = 0;//処理済みの文字数
	for (int i = 0; i < outputCount; ++i) {
		lineWidth += fontRenderer.XAdvance(text[i]);
		//改行文字か、行の幅が表示領域の幅を超えたら1行表示して改行
		if (text[i] == L'\n' || lineWidth > textAreaSize.x) {
			rowList.push_back(text.substr(outputOffset, i - outputOffset));
			outputOffset = i;
			lineWidth = 0;
		}
	}
	//表示すべき文字が残っていたら、それを追加
	if (outputOffset < outputCount) {
		rowList.push_back(text.substr(outputOffset, outputCount - outputOffset));
	}

	//最初に表示する行の位置(offset)を計算
	glm::vec2 offset = textAreaSize * glm::vec2(-0.5f, 0.5f);
	offset += textAreaOffset;
	offset.y -= fontRenderer.LineHeight();
	offset += position;

	//表示開始行を決める
	const int maxLines = static_cast<int>(textAreaSize.y / fontRenderer.LineHeight());
	int startLine = static_cast<int>(rowList.size()) - maxLines;
	if (startLine < 0) {
		startLine = 0;
	}
	//文章をフォント・レンダラーに設定
	fontRenderer.BeginUpdate();
	fontRenderer.Color(glm::vec4(0, 0, 0, 1));
	for (int i = startLine; i < static_cast<int>(rowList.size()); ++i) {
		fontRenderer.AddString(glm::vec3(offset, 0), rowList[i].c_str());
		offset.y -= fontRenderer.LineHeight();
	}
	fontRenderer.EndUpdate();
}

/*
テキスト・ウィンドウを描画する
*/
void TextWindow::Draw()
{
	if (!isOpen) {
		return;
	}
	const GLFWEW::Window& window = GLFWEW::Window::Instance();
	const glm::vec2 screenSize(window.Width(), window.Height());
	spriteRenderer.Draw(screenSize);
	fontRenderer.Draw(screenSize);
}

/*
テキストウィンドウを開く

@param str ウィンドウに表示する文章
*/
void TextWindow::Open(const wchar_t*str) 
{
	SetText(str);
	isOpen = true;
}

/*
テキストを変更する

@param str ウィンドウに表示する文章
*/
void TextWindow::SetText(const wchar_t*str)
{
	text = str;
	outputCount = 0;
	outputTimer = 0;
}

/*
テキストウィンドウを閉じる
*/
void TextWindow::Close()
{
	isOpen = false;
}

/*
文章の表示が完了したか調べる

@retval true  すべて文字を表示した
@retval false まだ表示してない文字がある
*/
bool TextWindow::IsFinished()const
{
	return isOpen && (outputCount >= static_cast<int>(text.size()));
}