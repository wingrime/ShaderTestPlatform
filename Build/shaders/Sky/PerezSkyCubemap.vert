#version 330
uniform mat4 matrixProjection;
uniform mat4 model;
uniform mat4 view;

in vec3 normal;
in vec3 position;
in vec2 UV;

out vec3 t_normal_gs;
out vec3 o_pos_v_gs;

void main(void)
{
	o_pos_v_gs =position;
	t_normal_gs = normalize(normal);
}
