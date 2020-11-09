/*
@file Image.cpp
*/
#include "Image.h"
void set_image(int no, float x, float y, const char* filename)
{
	if (no < 0 || no >= static_cast<int>(spriteBuffer.size())) {
		return;
	}

	auto itr = textureCache.find(filename);
	TexturePtr tex;
	if (itr != textureCache.end()) {
		tex = itr->second;
	}
	else {
		std::string str;
		str.reserve(1024);
		str += "Res/";
		str += filename;
		tex = Texture::LoadFromFile(str.c_str());
		if (tex) {
			textureCache.emplace(filename, tex);
		}
	}

	if (tex) {
		spriteBuffer[no].Texture(tex);
		spriteBuffer[no].Position(glm::vec3(x, y, 0));
		spriteBuffer[no].Scale(glm::vec2(1, 1));
		spriteBuffer[no].Rotation(0);
		spriteBuffer[no].Shear(0);
		spriteBuffer[no].Color(glm::vec4(1, 1, 1, 1));
		spriteBuffer[no].ColorMode(BlendMode_Multiply);

		spriteBuffer[no].init_action();
	}
}
