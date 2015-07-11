/*
AS IS display shader
*/
#version 330
in vec2 uv;
layout(location = 0) out vec4 color;
uniform sampler2D texSRC1;
#define texFB texSRC1
void main ()
{
  	vec3 img_color = texture(texFB,uv).rgb;
	color = vec4(img_color,1.0);
}
