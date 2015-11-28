#version 330
uniform mat4 cam_proj;
uniform mat4 model;
uniform mat4 view;
uniform vec4 light;

uniform sampler2D dM;
in vec3 normal;
in vec3 position;
in vec2 UV;
out vec4 n;
out vec4 p;
out vec2 uv;
out vec4 lp;
void main(void)
{
mat4 M = model;
mat4 MV = view*model;
mat4 MVP = (cam_proj)*MV;

n = ((MV)*vec4(normal,0.0));

p = ((MV)*vec4(position,0.0));

float near = 0.001;
float far = 10000000;
gl_Position  = MVP*vec4(position,1.0);
gl_Position.z  = 2.0*log(gl_Position.w/near)/log(far/near) -1;
gl_Position.z *= gl_Position.w;
//[y][z][x]
lp  = (MV)*vec4(light);
//lp  = vec4(-10.5,0,-25,0);
uv = UV;
}
