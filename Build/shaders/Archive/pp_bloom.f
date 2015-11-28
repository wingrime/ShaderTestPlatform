#version 330
uniform vec4 vp; /* viewport conf*/
layout(location = 0) out vec4 color;
uniform sampler2D texSRC1;
uniform sampler2D texSRC2;

#define texDEPTH texSRC2
#define texFB texSRC1
uniform float hdrBloomClamp = 0.0;
uniform float hdrBloomMul = 1.0;
void main ()
{
  vec2 c = gl_FragCoord.xy/vp.xy;
  vec4 img_color = texture(texFB,c);
  vec4 sum= texture2D(texFB, c);

if (length(sum.rgb) < hdrBloomClamp)
    sum = vec4(vec3(0.0),1.0);
	color = vec4(sum.rgb*vec3(hdrBloomMul),1.0);
}