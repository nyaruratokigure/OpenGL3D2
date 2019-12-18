/*
@file BrightPassFilter.frag
*/
#version 430

layout(location=1) in vec2 inTexCoord;

out vec4 fragColor;

uniform sampler2D texColor;

void main()
{
  fragColor = texture(texColor, inTexCoord);
  float brightness = max(fragColor.r, max(fragColor.g, fragColor.b));
  float contribution = max(brightness - 1.0, 0.0);
  fragColor.rgb *= contribution / max(brightness, 0.000001);
}