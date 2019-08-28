/*
@file Texture.h
*/
#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED
#include<GL/glew.h>
#include <memory>

namespace Texture {
	class Image2D;
	using Image2DPtr = std::shared_ptr<Image2D>;

	GLuint CreateImage2D(GLsizei width, GLsizei height, const GLvoid*data,
		GLenum format,GLenum type);
	GLuint LoadImage2D(const char*path);

	/*
	2Dテクスチャ
	*/
	class Image2D
	{
	public:
		static Image2DPtr Create(const char*);
		Image2D() = default;
		explicit Image2D(GLuint texId);
		~Image2D();

		void Reset(GLuint texId);
		bool IsNull() const;
		GLint Width()const { return width; }
		GLint Height()const { return height; }
		GLuint Get() const;

	private:
		GLuint id = 0;
		GLint width = 0;
		GLint height = 0;
	};
}//namespace Tecture



#endif // !TEXTURE_H_INCLUDED

