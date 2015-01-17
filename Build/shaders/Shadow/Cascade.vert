#version 330
uniform mat4 cam_proj;
uniform mat4 model;
uniform mat4 view;

in vec3 normal;
in vec3 position;
in vec2 UV;

out vec4 o_position;

void main(void)
{

mat4 M = model;
mat4 MV = view*model;
mat4 MVP = (cam_proj)*MV;

o_position = MVP*vec4(position,1.0);
gl_Position  = o_position;
o_position  =o_position/ o_position.w;
}
  