/**
* @file Shadow.frag
*/
#version 430

layout(location=1) in vec2 inTexCoord;

uniform sampler2D texColor;

void main()
{
  // �s�����x��0.5�����̃s�N�Z���͔j������(�e�𗎂Ƃ��Ȃ�).
  float a = texture(texColor, inTexCoord).a;
  if (a < 0.5) {
    discard;
  }
}