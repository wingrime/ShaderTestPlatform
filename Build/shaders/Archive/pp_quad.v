#version 330
/* PP shaders project AS-IS*/
noperspective in vec3 vCord;

void main(void)
{
    gl_Position = vec4(vCord,1.0f);

}