#version 440
/* Shadow Shader - Empty - depth only  */
layout(location = 0) out vec4 dstColor; /*albedo*/

in vec4 o_position;
void main() 
{
	dstColor = vec4( vec3(o_position),1.0);
}