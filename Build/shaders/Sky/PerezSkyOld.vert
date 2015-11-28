#version 330
uniform mat4 cam_proj;
uniform mat4 model;
uniform mat4 view;

uniform vec4 main_light_dir = vec4(0.0,1.0,0.0,0.0);

uniform mat4 sm_projection_mat;
uniform mat4 sm_view_mat;


in vec3 normal;
in vec3 position;
in vec2 UV;

out vec3 lightPos;
out vec3 vPos;
out vec2 uv;

out vec3 o_normal;
out vec3 o_pos_v;
out vec3 t_normal;
out vec3 t_tangent;
out vec3 t_binormal;

out mat4 sm_proj;
out mat4 sm_view;
out mat4 o_view;
out mat4 o_proj;
out vec4 o_light;
out mat4 sm_mat;


out mat4 MV_n;

void main(void)
{
o_pos_v =  position;
sm_proj = sm_projection_mat;
sm_view = sm_view_mat;

const mat4 bias = (mat4(0.5 , 0.0 , 0.0, 0.0 ,
					  0.0 , 0.5 , 0.0, 0.0 ,
					  0.0 , 0.0 , 0.5, 0.0 ,
					  0.5 , 0.5 , 0.5, 1.0 ));

sm_mat = bias*sm_proj*sm_view;
mat4 M = model;
mat4 MV = view*model;
//MV_n = transpose(inverse(MV));
// rigid transform only
MV_n = MV;
mat4 MVP = (cam_proj)*MV;
vec3 t_tangent;

t_normal = normalize((MV_n*vec4(normal,0.0)).xyz);
o_normal = normalize(normal);

o_view = MV;
o_proj = cam_proj;
vPos = (MV*vec4(position,1.0)).xyz;

/*BRDF Required vectors  */

lightPos =( MV*main_light_dir).xyz;
o_light = main_light_dir;
/*realUV*/
uv = vec2(1.0,-1.0)*UV;
gl_Position  = MVP*vec4(position,1.0);
}
