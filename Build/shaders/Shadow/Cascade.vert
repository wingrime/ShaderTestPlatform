#version 330
/*optimize*/
uniform mat4 MVP;
in vec3 normal;
in vec3 position;
in vec2 UV;
void main(void)
{
gl_Position  = vec4(position,1.0);

}
