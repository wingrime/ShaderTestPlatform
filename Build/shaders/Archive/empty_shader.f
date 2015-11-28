#version 330

out vec4 color;
in float depth;
void main ()
{

    color =  vec4(vec3(1.0)*depth*0.5+0.5,1.0 );

}