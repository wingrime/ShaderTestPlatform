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
  vec4 sum;
  const ivec2 filterDir = ivec2(1,0);
  sum = (texelFetchOffset(texFB, ivec2(gl_FragCoord.xy),0,filterDir*(-4))
  + texelFetchOffset(texFB, ivec2(gl_FragCoord.xy),0,filterDir*(4)))* 0.05;

  sum += (texelFetchOffset(texFB, ivec2(gl_FragCoord.xy),0,filterDir*(-3))
    + texelFetchOffset(texFB, ivec2(gl_FragCoord.xy),0,filterDir*(3)))* 0.09;

  sum += (texelFetchOffset(texFB, ivec2(gl_FragCoord.xy),0,filterDir*(-2))
    + texelFetchOffset(texFB, ivec2(gl_FragCoord.xy),0,filterDir*(2)))* 0.12;

  sum += (texelFetchOffset(texFB, ivec2(gl_FragCoord.xy),0,filterDir*(-1))
    + texelFetchOffset(texFB, ivec2(gl_FragCoord.xy),0,filterDir*(1)))* 0.15;
  
  sum += texelFetch(texFB, ivec2(gl_FragCoord.xy),0 )* 0.16;

  color = vec4(sum.rgb,1.0);
}