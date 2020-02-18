/*
@file Shader.h
*/
#ifndef SHADER_H_INCLUDED
#define SHADER_H_INCLUDED
#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <memory>


namespace Shader {

	class Program;
	using ProgramPtr = std::shared_ptr<Program>;

	GLuint Build(const GLchar* vsCode, const GLchar* fsCode);
	GLuint BuildFromFile(const char*vspath, const char*fspath);




	/*
	�V�F�[�_�[�E�v���O����
	*/
	class Program
	{
	public:
		static ProgramPtr Create(const char*vsPath, const char*fsPath);
		Program();
		explicit Program(GLuint programmId);
		~Program();

		void Reset(GLuint programId);
		bool IsNull() const;
		void Use();
		void BindTexture(GLuint, GLuint);
		void SetViewProjectionMatrix(const glm::mat4&);
		void SetInverseViewRotationMatrix(const glm::mat4& matView);
		void SetModelMatrix(const glm::mat4&);
		void SetPointLightIndex(int count, const int* indexList);
		void SetSpotLightIndex(int count, const int* indexList);
		void SetCameraPosition(const glm::vec3&);
		void SetTime(float);
		void SetViewInfo(float w, float h, float near, float far);
		void SetCameraInfo(float focalPlane, float focalLength, float aperture,
			float sensorSize);
		void SetBlurDirection(float x, float y);
		void SetShadowViewProjectionMatrix(const glm::mat4&);
		static const GLint shadowTextureBindingPoint = 16;

		///�v���O����ID���擾����
		GLuint Get()const { return id; }

	private:
		GLuint id = 0;//�v���O����ID

		//uniform�ϐ��̈ʒu
		GLint locMatMVP = -1;
		GLint locMatModel = -1;
		GLint locPointLightCount = -1;
		GLint locPointLightIndex = -1;
		GLint locSpotLightCount = -1;
		GLint locSpotLightIndex = -1;
		GLint locCameraPosition = -1;
		GLint locTime = -1;
		GLint locViewInfo = -1;
		GLint locCameraInfo = -1;
		GLint locBlurDirection = -1;
		GLint locMatShadow = -1;
		GLint locMatInverseViewRotation = -1;


		glm::mat4 matVP = glm::mat4(1);//�r���[�v���W�F�N�V�����s��
	};

}//Shader namespace

#endif //SHADER_H_INCLUDED