#version 430 core
uniform mat4 cam_proj;
uniform mat4 model;
uniform mat4 view;

uniform vec4 main_light_dir = vec4(0.0,1.0,0.0,0.0);

uniform mat4 sm_projection_mat;
uniform mat4 sm_view_mat;


in vec3 normal;
in vec3 position;
in vec2 UV;

out vec3 lightPos_gs;
out vec3 vPos_gs;
out vec2 uv_gs;

out vec3 o_normal_gs;
out vec3 o_pos_v_gs;
out vec3 t_normal_gs;
out vec3 t_tangent_gs;
out vec3 t_binormal_gs;

out mat4 sm_proj_gs;
out mat4 sm_view_gs;
out mat4 o_view_gs;
out mat4 o_proj_gs;
out vec4 o_light_gs;
out mat4 sm_mat_gs;


out mat4 MV_n;

void main(void)
{
o_pos_v_gs =  position;
sm_proj_gs = sm_projection_mat;
sm_view_gs = sm_view_mat;

const mat4 bias = (mat4(0.5 , 0.0 , 0.0, 0.0 ,
					  0.0 , 0.5 , 0.0, 0.0 ,
					  0.0 , 0.0 , 0.5, 0.0 ,
					  0.5 , 0.5 , 0.5, 1.0 ));

sm_mat_gs = bias*sm_proj_gs*sm_view_gs;
mat4 M = model;
mat4 MV = view*model;
//MV_n = transpose(inverse(MV));
// rigid transform only
MV_n = MV;
mat4 MVP = (cam_proj)*MV;
vec3 t_tangent;

t_normal_gs = normalize((MV_n*vec4(normal,0.0)).xyz);
o_normal_gs = normalize(normal);

o_view_gs = MV;
o_proj_gs = cam_proj;
vPos_gs = (MV*vec4( position,1.0)).xyz;

/*BRDF Required vectors  */

lightPos_gs =( MV*main_light_dir).xyz;

o_light_gs = main_light_dir;

/*realUV*/
uv_gs = vec2(1.0,-1.0)*UV;

//gl_Position  = MVP*vec4( position,1.0);
}
