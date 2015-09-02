#version 330
uniform vec4 vp; /* viewport conf*/
in vec2 uv;
layout(location = 0) out vec4 color;
uniform sampler2D texSRC1;
#define texFB texSRC1
uniform float hdrBloomClamp = 0.0;
uniform float hdrBloomMul = 1.0;
void main ()
{
  	vec3 img_color = texelFetch(texFB,ivec2(gl_FragCoord.xy*2.0),0).rgb;
    //vec3 img_color = texture(texFB,uv).rgb;
    //size depend, don't interpolate, just sample
  /*CCIR601*/
  	const vec3 lum_CCIR601 = vec3(0.299, 0.587, 0.114);
  	float lum = dot(img_color, lum_CCIR601);
	color = vec4(vec3(clamp((lum - hdrBloomClamp),0.0,1.0)),1.0);
}
