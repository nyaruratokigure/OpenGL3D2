/*
@file FramebufferObject.h
*/
#ifndef FRAMEBUFFEROBJECT_H_INCLUDED
#define FRAMEBUFFEROBJECT_H_INCLUDED
#include <GL/glew.h>
#include "Texture.h"
#include <memory>

class FrameBufferObject;
using FrameBufferObjectPtr = std::shared_ptr<FrameBufferObject>;

/*
�t���[���o�b�t�@�̎��
*/
enum class FrameBufferType {
	colorAndDepth,
	colorOnly,
	depthOnly,
};

/*
�t���[���o�b�t�@�I�u�W�F�N�g
*/
class FrameBufferObject
{
public:
	static FrameBufferObjectPtr Create(int w, int h,
		GLenum internalFormat =GL_RGBA8,FrameBufferType type =FrameBufferType::colorAndDepth);
	FrameBufferObject() = default;
	~FrameBufferObject();

	GLuint GetFramebuffer() const;
	const Texture::Image2DPtr& GetColorTexture() const;
	const Texture::Image2DPtr& GetDepthTexture() const;

private:
	Texture::Image2DPtr texColor; ///<�J���[�o�b�t�@�p�e�N�X�`��
	Texture::Image2DPtr texDepth; ///<�[�x�o�b�t�@�p�e�N�X�`��
	GLuint id = 0; ///< FBO��ID
};

#endif // FRAMEBUFFEROBJECT_H_INCLUDED
