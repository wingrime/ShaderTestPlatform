#version 330
/*Depth and normal pass, write only normal*/
layout(location = 0) out vec4 worldNormal;
in vec3 vWorldSpaceNormal;
void main() 
{
	worldNormal = vec4(0.5*normalize(vWorldSpaceNormal)+0.5,1.0);
}