#version 330
uniform vec4 vp; /* viewport conf*/
layout(location = 0) out vec4 color;
uniform sampler2D texSRC1;
#define texFB texSRC1
uniform float hdrBloomClamp = 0.0;
uniform float hdrBloomMul = 1.0;
void main ()
{
  	vec2 c = gl_FragCoord.xy/vp.xy;
  	vec3 img_color = texture(texFB,c).rgb;
  /*CCIR601*/
  	const vec3 lum_CCIR601 = vec3(0.299, 0.587, 0.114);
  	float lum = dot(img_color, lum_CCIR601);

	if (lum < hdrBloomClamp)
    	color = vec4(vec3(0.0),1.0);
   	else
		color = vec4(lum*vec3(hdrBloomMul),1.0);
}