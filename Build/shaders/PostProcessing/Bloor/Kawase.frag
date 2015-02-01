#version 330
uniform vec4 vp; /* viewport conf*/
layout(location = 0) out vec4 color;
in vec2 uv;
uniform sampler2D texSRC1;
uniform sampler2D texSRC2;

#define texDEPTH texSRC2
#define texFB texSRC1

uniform float blurSize = 1.0/1000.0;


void main ()
{
 float bS = blurSize/10.0;
  vec2 c = uv;
  float sum = 0.0;
  sum += texture(texFB, vec2(c.x - 1.0*bS, c.y - 1.0*bS)).r;
  sum += texture(texFB, vec2(c.x + 1.0*bS, c.y + 1.0*bS)).r;
  sum += texture(texFB, vec2(c.x - 1.0*bS, c.y + 1.0*bS)).r;
  sum += texture(texFB, vec2(c.x + 1.0*bS, c.y - 1.0*bS)).r;
  sum *= 0.25;
  color = vec4(vec3(sum),1.0);
}