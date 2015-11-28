#version 330
uniform vec4 vp; /* viewport conf*/
layout(location = 0) out vec4 color;
uniform sampler2D texSRC1;
uniform sampler2D texSRC2;
uniform sampler2D texSRC3;
#define light_depth_sampler texSRC2
#define screen_depth_sampler texSRC1
#define light_vector_sampler texSRC3
uniform mat4 sm_proj_matrix;
uniform mat4 sm_view_matrix;
uniform mat4 cam_proj_matrix;
uniform mat4 cam_view_matrix;


const int Samples = 4;
const float pi = 3.14159;
	

vec3 restore_pos_from_depth_view(float z,vec2 uv) {
	mat4 inv_p = inverse(cam_proj_matrix*cam_view_matrix);
        vec4 s_pos = vec4(uv*2.0-1.0 ,z*2.0-1.0,1.0 );
	s_pos = inv_p * s_pos;
	return s_pos.xyz/ s_pos.w;

}

vec3 restore_pos_from_depth_light(float z,vec2 uv) {
	mat4 inv_p = inverse(sm_proj_matrix*sm_view_matrix);
        vec4 s_pos = vec4(uv*2.0-1.0 ,z*2.0-1.0,1.0 );
	s_pos = inv_p * s_pos;
	return s_pos.xyz / s_pos.w;
}


float shadow_test(vec2 uv , float z) {
 float outP =  step(texture(light_depth_sampler,uv).r,z - 0.000005);
 if (uv.x > 0.0 && uv.x < 1.0  && uv.y > 0.0 && uv.y < 1.0)
 	return outP;
 	else
 	return 0;
}

/* Touh09*/
void main ()
{

	vec2 c = gl_FragCoord.xy/vp.xy;

	float light_depth =  texture(light_depth_sampler,c).r;
	float camera_depth =  texture(screen_depth_sampler,c).r;
        vec3 light_vector =  texture(light_vector_sampler,c).xyz;
	/* raymaching from  point to camera in light space*/
        vec3 tar = normalize(vec3(c.x,c.y,camera_depth));
        vec4 vd = (sm_proj_matrix*sm_view_matrix*vec4(restore_pos_from_depth_view(tar.z,tar.xy),1.0));

        //vec3 tar = normalize(vec3(c.x,c.y,camera_depth));
        //vec4 vd = (sm_proj_matrix*sm_view_matrix*vec4(restore_pos_from_depth_view(tar.z,tar.xy),1.0));
	vec3 viewDir =normalize(vd.xyz /vd.w  );
	float albedo = 1.0; //todo;
	float Phi = 1.0;
	//Phase function TODO
        float tau = 0.10;
        float dl =  0.01; //step size;
	
        float L0 = 0.5;//initial FLUX;
	

	vec4 x_ = (sm_proj_matrix*sm_view_matrix* vec4(restore_pos_from_depth_view(camera_depth,c),1.0));

	vec3 x = x_.xyz / x_.w;
	float s = length(x);
        float L =  L0 * exp (-s*tau);

/*
	for (float l = s - dl; l>= 0 ; l -= dl ) {
		x  = x+viewDir * vec3(dl);
		float v = shadow_test(x.xy,x.z) ; 
		float d = length (x);
		float Lin = exp(-d * tau) * v * Phi/4.0/pi/d/d;
		float Li = Lin * tau * 1.0;
		L += Li * exp(-l * tau) *dl;
	}
*/







	float res = 0.0;
        const int S  =100 ;

float l = s - dl;

	for (int i = 0 ; i < S ; i++) {
        x  = x+viewDir * dl;//(s/float(S));

	float d = length (x);
	float v =  shadow_test(x.xy,x.z) ; 
	float Lin = exp(-d * tau) * v * Phi/(4.0*pi*d*d);
	float Li = Lin * tau * 1.0;
	L += Li * exp(-l * tau) *dl;
	res += shadow_test(x.xy,x.z) ; 
	l -= dl ;

        if (l < 0)
                break;
	}
	res  = res / S;
	
	
        color = vec4(vec3(res),1.0);

     // color = vec4(restore_pos_from_depth_light(light_depth,c)-light_vector,1.0);

}
