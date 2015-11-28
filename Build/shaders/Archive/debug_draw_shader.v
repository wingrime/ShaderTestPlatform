/* Debug output shader for output lines */
#version 330
in vec3 pos;
uniform mat4 MVP;
void main(void)
{
    gl_Position = MVP*vec4(pos,1.0f);

}