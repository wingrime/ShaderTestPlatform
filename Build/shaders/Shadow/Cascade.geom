#version 330 core
layout (triangles) in;
layout (triangle_strip,max_vertices = 18) out;

uniform mat4 MVP0;
uniform mat4 MVP1;
uniform mat4 MVP2;
uniform mat4 MVP3;
void main(void)
{
    int i;

        
        gl_Layer = 0;
        for (i = 0; i < gl_in.length(); i++)
        { 
            gl_Position = MVP0*gl_in[i].gl_Position;
            EmitVertex();
        } 
        EndPrimitive();
        gl_Layer = 1;
        for (i = 0; i < gl_in.length(); i++)
        { 
            gl_Position = MVP1*gl_in[i].gl_Position;
            EmitVertex();
        } 
        EndPrimitive();
        gl_Layer = 2;
        for (i = 0; i < gl_in.length(); i++)
        { 
            gl_Position = MVP2*gl_in[i].gl_Position;
            EmitVertex();
        } 
        EndPrimitive();
        gl_Layer = 3;
        for (i = 0; i < gl_in.length(); i++)
        { 
            gl_Position = MVP3*gl_in[i].gl_Position;
            EmitVertex();
        } 
        EndPrimitive();
}
