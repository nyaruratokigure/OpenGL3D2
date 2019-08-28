/*
@file shader.cpp
*/
#include"Shader.h"
#include<glm/gtc/matrix_transform.hpp>
#include<vector>
#include<iostream>
#include<fstream>
#include<stdint.h>

/*
シェーダーに関する機能を格納する名前空間
*/
namespace Shader {
	/*
	シェーダー・プログラムをコンパイルする

	@param type シェーダーの種類
	@param string シェーダー・プログラムへのポインタ

	@retval 0より大きい 作成したシェーダー・オブジェクト
	@retval 0           シェーダー・オブジェクトの作成に失敗
	*/
	GLuint Compile(GLenum type, const GLchar*string) {
		if (!string) {
			return 0;
		}

		GLuint shader = glCreateShader(type);
		glShaderSource(shader, 1, &string, nullptr);
		glCompileShader(shader);
		GLint compiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		//コンパイルに失敗した場合、原因をコンソールに出力して0を返す
		if (!compiled) {
			GLint infoLen = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
			if (infoLen) {
				std::vector<char>buf;
				buf.resize(infoLen);
				if (static_cast<int>(buf.size()) >= infoLen) {
					glGetShaderInfoLog(shader, infoLen, NULL, buf.data());
					std::cerr << "ERROR:シェーダーのコンパイルに失敗.\n" << buf.data() << std::endl;
				}
			}
			glDeleteShader(shader);
			return 0;
		}
		return shader;
	}

/*
プログラム・オブジェクトを作成する

@param vsCode 頂点シェーダー・プログラムへのポインタ
@param fsCode フラグメントシェーダー・プログラムへのポインタ

@retval 0より大きい 作成したプログラム・オブジェクト
@retval 0           プログラム・オブジェクトの作成に失敗
*/
	GLuint Build(const GLchar*vsCode, const GLchar*fsCode) {
		GLuint vs = Compile(GL_VERTEX_SHADER, vsCode);
		GLuint fs = Compile(GL_FRAGMENT_SHADER, fsCode);
		if (!vs || !fs) {
			return 0;
		}
		GLuint program = glCreateProgram();
		glAttachShader(program, fs);
		glDeleteShader(fs);
		glAttachShader(program, vs);
		glDeleteShader(vs);
		glLinkProgram(program);
		GLint linkStatus = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
		if (linkStatus != GL_TRUE) {
			GLint infoLen = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
			if (infoLen) {
				std::vector<char>buf;
				buf.resize(infoLen);
				if (static_cast<int>(buf.size()) >= infoLen) {
					glGetProgramInfoLog(program, infoLen, NULL, buf.data());
					std::cerr << "ERROR:シェーダーのリンクに失敗.\n" << buf.data() << std::endl;
				}
			}
			glDeleteProgram(program);
			return 0;
		}
		return program;
	}

	/*
	ファイルを読み込む

	@param path 読み込むファイル名

	@return 読み込んだデータ
	*/
	std::vector<GLchar>ReadFile(const char*path) {
		std::basic_ifstream<GLchar>ifs;
		ifs.open(path, std::ios_base::binary);
		if (!ifs.is_open()) {
			std::cerr<< "ERROR:" << path << "を開けません.\n";
			return{};
		}
		ifs.seekg(0, std::ios_base::end);
		const size_t length = (size_t)ifs.tellg();
		ifs.seekg(0, std::ios_base::beg);
		std::vector<GLchar>buf(length);
		ifs.read(buf.data(), length);
		buf.push_back('\0');
		return buf;
	}

	/*
	ファイルからプログラム・オブジェクトを作成する

	@param vsPath 頂点シェーダー・ファイル名
	@param fsPath フラグメントシェーダー・ファイル名

	@return 作成したプログラム・オブジェクト
	*/
	GLuint BuildFromFile(const char*vsPath, const char*fsPath) {
		const std::vector<GLchar>vsCode = ReadFile(vsPath);
		const std::vector<GLchar>fsCode = ReadFile(fsPath);
		return Build(vsCode.data(), fsCode.data());
	}

	/*
	ライトリストを初期化する

	全ての光源の明るさを0にする
	*/
	void LightList::Init() {
		ambient.color = glm::vec3(0);
		directional.color = glm::vec3(0);
		for (int i = 0; i < 4; ++i) {
			point.color[i] = glm::vec3(0);
		}
		for (int i = 0; i < 4; ++i) {
			spot.color[i] = glm::vec3(0);
		}
	}

	/*
	コンストラクタ
	*/
	Program::Program() {
		lights.Init();
	}


	/*
	コンストラクタ

	@param programID プログラムオブジェクトのID
	*/
	Program::Program(GLuint programId) {
		lights.Init();
		Reset(programId);
	}

	/*
	デストラクタ

	プログラムオブジェクトを削除する
	*/
	Program::~Program()
	{
		glDeleteProgram(id);
	}

	/*
	プログラムオブジェクトを設定する

	@param id プログラムオブジェクトのID
	*/
	void Program::Reset(GLuint programId) {
		if (programId == id) {
			std::cerr << "[エラー] 同じプログラムIDが指定されました.\n";
			return;
		}
		glDeleteProgram(id);
		id = programId;
		if (id == 0) {
			locMatMVP = -1;
			locAmbLightCol = -1;
			locDirLightDir = -1;
			locDirLightCol = -1;
			locPointLightPos = -1;
			locPointLightCol = -1;
			locSpotLightDir = -1;
			locSpotLightPos = -1;
			locSpotLightCol = -1;
			return;
		}

		locMatMVP = glGetUniformLocation(id, "matMVP");
		locAmbLightCol = glGetUniformLocation(id, "ambientLight.color");
		locDirLightDir = glGetUniformLocation(id, "directionalLight.direction");
		locDirLightCol = glGetUniformLocation(id, "directionalLight.color");
		locPointLightPos = glGetUniformLocation(id, "pointLight.position");
		locPointLightCol = glGetUniformLocation(id, "pointLight.color");
		locSpotLightDir = glGetUniformLocation(id, "spotLight.dirAndCutOff");
		locSpotLightPos = glGetUniformLocation(id, "spotLight.posAndInnerCutOff");
		locSpotLightCol = glGetUniformLocation(id, "spotLight.color");

		const GLint texColorLoc = glGetUniformLocation(id, "texColor");
		if (texColorLoc >= 0) {
			glUseProgram(id);
			glUniform1i(texColorLoc, 0);
		}
	}

	/*
	プログラムオブジェクトが設定されているか調べる

	@retval true  設定されている
	@retval false 設定されていない
	*/
	bool Program::IsNull() const {
		return id == 0;
	}


	/*
	プログラムオブジェクトをグラフィックパイプラインに割り当てる
	*/

	void Program::Use() {
		if (id) {
			glUseProgram(id);
		}
	}

	/*
	描画に使用するテクスチャを設定する

	@param unitNo 設定するテクスチャイメージユニットの番号(0~)
	@param texId  設定するテクスチャのID
	*/
	void Program::BindTexture(GLuint uintNo, GLuint texId) {
		glActiveTexture(GL_TEXTURE0 + uintNo);
		glBindTexture(GL_TEXTURE_2D, texId);
	}

	/*
	描画に使われるライトを設定する

	@param lights 設定するライト
	*/
	void Program::SetLightList(const LightList& lights)
	{
		this->lights = lights;

		//ライトの色情報をGPUメモリに転送する
		if (locAmbLightCol >= 0) {
			glUniform3fv(locAmbLightCol, 1, &lights.ambient.color.x);
		}
		if (locDirLightCol >= 0) {
			glUniform3fv(locDirLightCol, 1, &lights.directional.color.x);
		}
		if (locPointLightCol >= 0) {
			glUniform3fv(locPointLightCol, 8, &lights.point.color[0].x);
		}
		if (locSpotLightCol >= 0) {
			glUniform3fv(locSpotLightCol, 4, &lights.spot.color[0].x);
		}
	}
	/*
	描画に使われるビュープロジェクション行列

	@param matMV 設定するビュープロジェクション行列
	*/
	void Program::SetViewProjectionMatrix(const glm::mat4& matVP) {
		this->matVP = matVP;
		if (locMatMVP >= 0) {
			glUniformMatrix4fv(locMatMVP, 1, GL_FALSE, &matVP[0][0]);
		}
	}

	/*
	メッシュを描画する

	@param mesh		　描画するメッシュ
	@oaram translate　平行移動量
	@param rotate	  回転角度(ラジアン)
	@param scale	  拡大縮小率(1=等倍,0.5=1/2倍,2.0=2倍)

	この関数を使う前に、Use()を実行しておくこと
	*/
	void Program::Draw(const Mesh& mesh,
		const glm::vec3& translate, const glm::vec3& rotate, const glm::vec3& scale)
	{
		if (id == 0) {
			return;
		}
		//モデル行列を計算する
		const glm::mat4 matScale = glm::scale(glm::mat4(1), scale);
		const glm::mat4 matRotateY = glm::rotate(glm::mat4(1), rotate.y, glm::vec3(0, 1, 0));
		const glm::mat4 matRotateZY = glm::rotate(matRotateY, rotate.z, glm::vec3(0, 0, -1));
		const glm::mat4 matRotateXZY = glm::rotate(matRotateZY, rotate.x, glm::vec3(1, 0, 0));
		const glm::mat4 matTranslate = glm::translate(glm::mat4(1), translate);
		const glm::mat4 matModelTR = matTranslate * matRotateXZY;
		const glm::mat4 matModel = matModelTR*matScale;

		//モデルビュープロジェクション行列を計算し、GPUメモリに転送する
		const glm::mat4 matMVP = matVP * matModel;
		glUniformMatrix4fv(locMatMVP, 1, GL_FALSE, &matMVP[0][0]);

		//指向性ライトの向きをモデル座標系に変換して、GPUメモリに転送する
		if (locDirLightDir >= 0) {
			const glm::mat3 matInvRotate = glm::inverse(glm::mat3(matRotateXZY));
			const glm::vec3 dirLightDirOnModel = matInvRotate * lights.directional.direction;
			glUniform3fv(locDirLightDir, 1, &dirLightDirOnModel.x);
		}
		//モデル座標系におけるポイント・ライトの座標を計算して、GPUメモリに転送する
		if (locPointLightPos >= 0) {
			const glm::mat4 matInvModel = glm::inverse(matModelTR);
			glm::vec3 pointLightPosOnModel[8];
			for (int i = 0; i < 8; ++i) {
				pointLightPosOnModel[i] = matInvModel * glm::vec4(lights.point.position[i], 1);
			}
			glUniform3fv(locPointLightPos, 8, &pointLightPosOnModel[0].x);
		}
		//モデル座標系におけるスポットライトの座標を計算して、GPUメモリに転送する
		if (locSpotLightDir >= 0 && locSpotLightPos>=0) {
			const glm::mat3 matInvRotate = glm::inverse(glm::mat3(matRotateXZY));
			const glm::mat4 matInvModel = glm::inverse(matModelTR);
			glm::vec4 spotLightDirOnModel[4];
			glm::vec4 spotLightPosOnModel[4];
			for (int i = 0; i < 4; ++i) {
				const glm::vec3 invDir = matInvRotate * glm::vec3(lights.spot.dirAndCutOff[i]);
				spotLightDirOnModel[i] = glm::vec4(invDir, lights.spot.dirAndCutOff[i].w);
				const glm::vec3 pos = lights.spot.posAndInnerCutOff[i];
				spotLightPosOnModel[i] = matInvModel * glm::vec4(pos, 1);
				spotLightPosOnModel[i].w = lights.spot.posAndInnerCutOff[i].w;
			}
			glUniform4fv(locSpotLightDir, 4, &spotLightDirOnModel[0].x);
			glUniform4fv(locSpotLightPos, 4, &spotLightPosOnModel[0].x);
		}
	}

	/*
	プログラムオブジェクトを作成する

	@param vsPath 頂点シェーダーファイル名
	@param fsPath フラグメントシェーダーファイル名

	@return 作成したプログラムオブジェクト
	*/
	ProgramPtr Program::Create(const char*vsPath, const char*fsPath)
	{
		return std::make_shared<Program>(BuildFromFile(vsPath, fsPath));
	}

}//Shader namespace