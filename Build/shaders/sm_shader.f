/* Shadow Shader */
#version 330

layout(location = 0) out vec4 dstColor; /*albedo*/
layout(location = 1) out vec4 worldNormal; /*normal + z */
layout(location = 2) out vec4 worldPos; /*normal + z */

const float pi = 3.14159;

in mat4 sm_proj;
in mat4 sm_view;

in vec3 lightPos;
in vec3 vPos;
in vec2 uv;

uniform int mesh_flags;

/* Source texture */
uniform sampler2D texIMG;
uniform sampler2D texBUMP;
/* Shadow map depth buffer*/
uniform sampler2D sm_depth_sampler;


in vec3 t_normal;
in vec3 t_tangent;
in vec3 t_binormal;

in vec3 o_normal;
in vec3 o_pos_v;
in mat4 o_view;
in mat4 o_proj;
in mat4 MV_n;
in vec4 o_light;


void main() 
{

	float lv = dot(normalize(t_normal) , normalize(lightPos-vPos));
	dstColor = vec4(vec3(lv)*texture(texIMG,uv).rgb,1.0);
	worldNormal = vec4(o_normal,gl_FragCoord.z); /* world normal (now local)*/
	worldPos = vec4(o_pos_v,1.0); /* position (now local) */

}