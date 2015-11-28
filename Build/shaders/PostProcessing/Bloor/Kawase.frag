#version 330
uniform vec4 vp; /* viewport conf*/
layout(location = 0) out vec4 color;
in vec2 uv;
uniform sampler2D texSRC1;
#define texFB texSRC1

uniform float blurSize = 1.0/1000.0;


void main ()
{
  float bS = blurSize/2.5;
  float sum = 0.0;
  sum += texture(texFB, vec2(uv.x - 1.0*bS, uv.y - 1.0*bS)).r;
  sum += texture(texFB, vec2(uv.x + 1.0*bS, uv.y + 1.0*bS)).r;
  sum += texture(texFB, vec2(uv.x - 1.0*bS, uv.y + 1.0*bS)).r;
  sum += texture(texFB, vec2(uv.x + 1.0*bS, uv.y - 1.0*bS)).r;
  
  sum += texture(texFB, vec2(uv.x , uv.y - 2.0*bS)).r;
  sum += texture(texFB, vec2(uv.x,  uv.y + 2.0*bS)).r;
  sum += texture(texFB, vec2(uv.x - 2.0*bS, uv.y)).r;
  sum += texture(texFB, vec2(uv.x + 2.0*bS, uv.y)).r;
  sum *= 0.125;
  color = vec4(vec3(sum),1.0);
}