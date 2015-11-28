#version 430 core
layout (triangles) in;
layout (triangle_strip ) out;
layout (max_vertices = 18) out;
/*pass*/
in mat4 sm_proj_gs[];
in mat4 sm_view_gs[];


in vec3 lightPos_gs[];
in vec3 vPos_gs[];
in vec2 uv_gs[];
in vec3 t_normal_gs[];
in vec3 t_tangent_gs[];
in vec3 t_binormal_gs[];

in vec3 o_normal_gs[];
in vec3 o_pos_v_gs[];
in mat4 o_view_gs[];
in mat4 o_proj_gs[];
in mat4 MV_n_gs[];
in vec4 o_light_gs[];
in mat4 sm_mat_gs[];
in mat4 MV_n_gs[];

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



/**
in vec3 lightPos;
in vec3 vPos;
in vec2 uv;
in vec3 t_normal;

*/

void main(void)
{

	mat4 rot[6];
	/*
	// +X //
	 rot[0] = (mat4 ( 	0 	,  0  	, -1.0 	, 0 , 
	 			     	0 	,  1.0	, 0   	, 0 ,
				    	1.0 ,  0  	, 0   	, 0 ,
				     	0 	,  0  	, 0   	, 1.0 )) ;
	// -X /
	 rot[0] = (mat4 ( 	0 	,  0  	, 1.0 	, 0 , 
	 			     	0 	,  1.0	, 0   	, 0 ,
				    	-1.0 ,  0  	, 0   	, 0 ,
				     	0 	,  0  	, 0   	, 1.0 )) ;
	 //+Y/
	 rot[3] = (mat4 (  1 	,  0  	, 0 	, 0 , 
	 			     	0 	,  0	, -1.0 	, 0 ,
				    	0 	,  1.0	, 0   	, 0 ,
				     	0 	,  0  	, 0   	, 1.0 )) ;
	 //-Y/
	rot[0] = (mat4 ( 	-1.0 	,  0  	, 0 	, 0 , 
	 			     	0 	,  0	, -1.0 	, 0 ,
				    	0 	,  1.0	, 0   	, 0 ,
				     	0 	,  0  	, 0   	, 1.0 )) ;
	 //+z/
	 rot[5] = (mat4 ( 	1 	,  0  	, 0 	, 0 , 
	 			     	0 	,  1	, 0 	, 0 ,
				    	0 	,  0	, -1   	, 0 ,
				     	0 	,  0  	, 0   	, 1.0 )) ;
	 //-z/
	 rot[4] = (mat4 ( 	1 	,  0  	, 0 	, 0 , 
	 		     		0 	,  1	, 0 	, 0 ,
			    		0 	,  0	, 1   	, 0 ,
			     		0 	,  0  	, 0   	, 1.0 )) ;
    
	*/
/*	
	// +X
	rot[0] = (mat4 ( 	0 	,  0  	, -1.0 	, 0 , 
	 			     			0 	,  1.0	, 0   	, 0 ,
				    			1.0 ,  0  	, 0   	, 0 ,
				     			0 	,  0  	, 0   	, 1.0 )) ;
	// -X /
	 rot[1] = (mat4 ( 	0 	,  0  	, 1.0 	, 0 , 
	 			     			0 	,  1.0	, 0   	, 0 ,
				    			-1.0 ,  0  	, 0   	, 0 ,
				     			0 	,  0  	, 0   	, 1.0 )) ;
	 //+Y/
	 rot[2] = (mat4 ( 	1 	,  0  	, 0 	, 0 , 
	 			     			0 	,  0	, 1.0 	, 0 ,
				    			0 	,  -1.0	, 0   	, 0 ,
				     			0 	,  0  	, 0   	, 1.0 )) ;
	 //-Y/
	rot[3] =(mat4 ( 	1 	,  0  	, 0 	, 0 , 
	 			     			0 	,  0	, -1.0 	, 0 ,
				    			0 	,  1.0	, 0   	, 0 ,
				     			0 	,  0  	, 0   	, 1.0 )) ;
	 //+z/
	 rot[4] = (mat4 ( 	-1 	,  0  	, 0 	, 0 , 
	 			     			0 	,  1	, 0 	, 0 ,
				    			0 	,  0	, -1   	, 0 ,
				     			0 	,  0  	, 0   	, 1.0 )) ;
	 //-z/
	 rot[5] = (mat4 ( 	1 	,  0  	, 0 	, 0 , 
	 		     				0 	,  1	, 0 	, 0 ,
			    				0 	,  0	, 1   	, 0 ,
			     				0 	,  0  	, 0   	, 1.0 )) ;

*/
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
    		// camera coordinate  system
            gl_Position = o_proj_gs[i]*rot[layer]*o_view_gs[i]*vec4(o_pos_v_gs[i],1.0);
            // world
            gl_Layer = layer;
          	//gl_Position = o_proj_gs[i]*rot[layer]*vec4(o_pos_v_gs[i],1.0);
          	//gl_Position = o_proj_gs[i]*o_view_gs[i]*vec4(o_pos_v_gs[i],1.0);
            //TODO add rotation
 			lightPos =   (rot[layer]*vec4(lightPos_gs[i],1.0)).xyz; 
		 	vPos =  (rot[layer]*vec4(vPos_gs[i],1.0)).xyz; 
 			uv = uv_gs[i];

			t_normal = (rot[layer]*vec4(t_normal_gs[i],1.0)).xyz;
            EmitVertex();
        } 
        EndPrimitive();
    }
   
}
