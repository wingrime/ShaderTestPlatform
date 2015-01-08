#version 330
in vec3 initial_vertex_coord;

void main(void)
{
    gl_Position = vec4(initial_vertex_coord,1.0f);

}