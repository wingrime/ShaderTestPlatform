#version 330
uniform vec4 vp; /* viewport conf*/
layout(location = 0) out vec4 color;
noperspective in vec2 uv;
uniform sampler2D texSRC1;
uniform sampler2D texSRC2;

#define texDEPTH texSRC2
#define texFB texSRC1

void main ()
{
  vec2 c = uv;
  vec4 sum = vec4(0.0);

  sum += texelFetch(texFB, ivec2(gl_FragCoord.xy) + ivec2(0,-4),0 )* 0.05;
  sum += texelFetch(texFB, ivec2(gl_FragCoord.xy) + ivec2(0,-3),0 )* 0.09;
  sum += texelFetch(texFB, ivec2(gl_FragCoord.xy) + ivec2(0,-2),0 )* 0.12;
  sum += texelFetch(texFB, ivec2(gl_FragCoord.xy) + ivec2(0,-1),0 )* 0.15;
  sum += texelFetch(texFB, ivec2(gl_FragCoord.xy) + ivec2(0, 0),0 )* 0.16;
  sum += texelFetch(texFB, ivec2(gl_FragCoord.xy) + ivec2(0, 1),0 )* 0.15;
  sum += texelFetch(texFB, ivec2(gl_FragCoord.xy) + ivec2(0, 2),0 )* 0.12;
  sum += texelFetch(texFB, ivec2(gl_FragCoord.xy) + ivec2(0, 3),0 )* 0.09;
  sum += texelFetch(texFB, ivec2(gl_FragCoord.xy) + ivec2(0, 4),0 )* 0.05;

  color = vec4(sum.rgb,1.0);
}