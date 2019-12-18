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
フレームバッファの種類
*/
enum class FrameBufferType {
	colorAndDepth,
	colorOnly,
	depthOnly,
};

/*
フレームバッファオブジェクト
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
	Texture::Image2DPtr texColor; ///<カラーバッファ用テクスチャ
	Texture::Image2DPtr texDepth; ///<深度バッファ用テクスチャ
	GLuint id = 0; ///< FBOのID
};

#endif // FRAMEBUFFEROBJECT_H_INCLUDED
