#version 330
uniform mat4 MVP;

in vec3 normal;
in vec3 position;
in vec2 UV;


out vec3 o_normal;

void main(void)
{
o_normal = normalize(normal);
/*
0.0 in w make sky unaffected to translate,
witch means, where are you be, sky will be same
*/
gl_Position  = MVP*vec4(position,0.0);
}
