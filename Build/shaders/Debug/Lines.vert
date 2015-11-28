#version 330
/* Debug output shader for output lines */
in vec3 pos;
uniform mat4 MVP;
void main(void)
{
    gl_Position = MVP*vec4(pos,1.0f);

}
