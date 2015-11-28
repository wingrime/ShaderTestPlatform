#version 330
in vec3 vCord;

void main(void)
{
    gl_Position = vec4(vCord,1.0f);

}