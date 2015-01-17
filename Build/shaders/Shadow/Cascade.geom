#version 430 core
layout (triangles) in;
layout (triangle_strip ) out;
layout (max_vertices = 18) out;

uniform mat4 cam_proj;
uniform mat4 model;
uniform mat4 view;

in vec3 normal[];
in vec3 position[];
in vec2 UV[];

in vec3 o_normal[];
out vec3 gs_o_normal;
void main(void)
{
/*pass*/
mat4 M = model;
mat4 MV = view*model;
mat4 MVP = (cam_proj)*MV;
/*BRDF Required vectors  */



	//for (int layer = 0; layer < 6; layer++) {
		
		
    	for (int i = 0; i < 3; i++)
    	{	gl_Position  = MVP*vec4(position[i],1.0);
    		gs_o_normal = o_normal[i];
            //gl_Layer = layer;
            EmitVertex();
        } 
        EndPrimitive();
   // }
   
}
