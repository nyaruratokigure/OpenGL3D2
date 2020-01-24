/**
* @file Shadow.frag
*/
#version 430

layout(location=1) in vec2 inTexCoord;

uniform sampler2D texColor;

void main()
{
  // 不透明度が0.5未満のピクセルは破棄する(影を落とさない).
  float a = texture(texColor, inTexCoord).a;
  if (a < 0.5) {
    discard;
  }
}