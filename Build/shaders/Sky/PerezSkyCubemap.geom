#version 330 core
layout (triangles) in;
layout (triangle_strip ) out;
layout (max_vertices = 18) out;

in vec3 t_normal_gs[];
in vec3 o_pos_v_gs[];

out vec3 lightPos;
out vec3 vPos;
out vec2 uv;
out vec3 t_normal;


uniform mat4 matrixProjection;
uniform mat4 matrixModel;
uniform mat4 matrixView;

void main(void)
{

	mat4 rot[6];

	// +X
	 		rot[0] = transpose(mat4 ( 	0 	,  0  	, 1.0 	, 0 , 
	 			     			0 	,  1.0	, 0   	, 0 ,
				    			-1.0 ,  0  	, 0   	, 0 ,
				     			0 	,  0  	, 0   	, 1.0 )) ;
	// -X /
	 		rot[1] = transpose(mat4 ( 	0 	,  0  	, -1.0 	, 0 , 
	 			     			0 	,  1.0	, 0   	, 0 ,
				    			1.0 ,  0  	, 0   	, 0 ,
				     			0 	,  0  	, 0   	, 1.0 )) ;
	 //+Y/
	 		rot[3] = transpose(mat4 ( 	-1 	,  0  	, 0 	, 0 , 
	 			     			0 	,  0	, -1.0 	, 0 ,
				    			0 	,  -1.0	, 0   	, 0 ,
				     			0 	,  0  	, 0   	, 1.0 )) ;
	 //-Y/
			rot[2] =transpose(mat4 ( 	-1 	,  0  	, 0 	, 0 , 
	 			     			0 	,  0	, 1.0 	, 0 ,
				    			0 	,  1.0	, 0   	, 0 ,
				     			0 	,  0  	, 0   	, 1.0 )) ;
	 //+z/
	 		rot[4] = transpose(mat4 ( 	-1 	,  0  	, 0 	, 0 , 
	 			     			0 	,  1	, 0 	, 0 ,
				    			0 	,  0	, -1   	, 0 ,
				     			0 	,  0  	, 0   	, 1.0 )) ;
	 //-z/
	 		rot[5] = transpose(mat4 ( 	1 	,  0  	, 0 	, 0 , 
	 		     				0 	,  1	, 0 	, 0 ,
			    				0 	,  0	, 1   	, 0 ,
			     				0 	,  0  	, 0   	, 1.0 )) ;
    
    
    int i,layer;

	for (layer = 0; layer < 6; layer++) {
		
		
    	for (i = 0; i < 3; i++)
    	{	

            gl_Position = matrixProjection*rot[layer]*matrixView*matrixModel*vec4(o_pos_v_gs[i],1.0);

            gl_Layer = layer;

			t_normal = (rot[layer]*vec4(t_normal_gs[i],1.0)).xyz;
            EmitVertex();
        } 
        EndPrimitive();
    }
   
}
