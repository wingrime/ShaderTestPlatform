#version 330
/*optimize*/
uniform mat4 MVP;
in vec3 normal;
in vec3 position;
in vec2 UV;

void main(void)
{

//mat4 M = model;
//mat4 MV = view*model;
//MV_n = transpose(inverse(MV));
//mat4 MVP = (cam_proj)*view*model;
//vec3 t_tangent;


//t_normal = normalize((MV_n*vec4(normal,0.0)).xyz);
//o_normal = normalize(normal);

//vPos = (MV*vec4(position,1.0)).xyz;

/*BRDF Required vectors  */


//lightPos =( MV*light).xyz;
/*realUV*/
//uv = vec2(1.0,-1.0)*UV;
gl_Position  = MVP*vec4(position,1.0);
}
