#version 330
/*optimize*/
uniform mat4 MVP;
uniform mat4 MV;

in vec3 normal;
in vec3 position;
in vec2 UV;
out vec3 vWorldSpaceNormal;
void main(void)
{

vWorldSpaceNormal = normalize((MV*vec4(normal,0.0)).xyz);
gl_Position  = MVP*vec4(position,1.0);
}
