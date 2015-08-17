#version 330
/* PP shaders project AS-IS*/
in vec2 vCord;
noperspective out vec2 uv;

void main(void)
{
    gl_Position = vec4(vCord,1.0,1.0f); 
    uv  = vCord*0.5+0.5;

}