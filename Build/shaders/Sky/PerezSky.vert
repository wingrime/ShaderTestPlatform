#version 330
uniform mat4 cam_proj;
uniform mat4 model;
uniform mat4 view;




in vec3 normal;
in vec3 position;
in vec2 UV;


out vec3 o_normal;

void main(void)
{

mat4 M = model;
mat4 MV = view*model;

mat4 MVP = (cam_proj)*MV;


o_normal = normalize(normal);

/*BRDF Required vectors  */

gl_Position  = MVP*vec4(position,1.0);
}
