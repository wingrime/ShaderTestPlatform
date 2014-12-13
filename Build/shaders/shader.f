/* Base Lighting Shader */
#version 330
layout(location = 0) out vec4 dstColor;
layout(location = 1) out vec4 worldNormal; /*SSAO*/ 
const float pi = 3.14159;


/* BRDF Required vectors*/
in mat4 sm_proj;
in mat4 sm_view;

in vec3 lightPos;
in vec3 vPos;
in vec2 uv;
in vec3 t_normal;
in vec3 t_tangent;
in vec3 t_binormal;

in vec3 o_normal;
in vec3 o_pos_v;
in mat4 o_view;
in mat4 o_proj;
in mat4 MV_n;
in vec4 o_light;
in mat4 sm_mat;
uniform int mesh_flags;
const int MESH_V_TC_N = 1;
const int MESH_V_TC = 0;
/* Source texture */
uniform float bump_amount = 50.0;
uniform sampler2D texIMG;
uniform sampler2D texBUMP;
uniform sampler2D texture_alpha_sampler;
/* Shadow map depth buffer*/
uniform sampler2D sm_depth_sampler;

uniform sampler2D rsm_normal_sampler;
uniform sampler2D rsm_vector_sampler;
uniform sampler2D rsm_albedo_sampler;

uniform float lightIntensity = 1.0;
uniform float shadowPenumbra = 200.0;
uniform float ambientIntensity = 0.1;
vec3 F_schlick(vec3 l, vec3 n,vec3 c_spec) {

	return clamp(c_spec+ (vec3(1.0f)-c_spec)*pow(1.0- max(dot(l,n),0.0),0.05),0.0,1.0); 
}
/*lambertian cosine law with normalization factor*/
float lambert(vec3 n , vec3 l)  {
	return max(0.0f, dot(n,l));
}
/* orig */
float blinn(vec3 n, vec3 h, float p) {
	return pow(max(0.0f,dot (n,h)),p);
}
vec3 binn_phong(vec3 n, vec3 h, vec3 l, float alpha,vec3 c_diff,vec3 c_spec ) {

	vec3 cd = (vec3(1.0) - c_spec)*c_diff;
	float nf = (alpha + 2.0) / 8.0;
	return F_schlick(h,l,vec3(c_spec))*nf*pow(dot(n,h), alpha) *c_diff*(dot(n,l));
}



float G1V(float dotNV, float k)
{
	return 1.0f/(dotNV*(1.0f-k)+k);
}

float LightingFuncGGX_OPT2(vec3 N, vec3 V, vec3 L, float roughness, float F0)
{
	float alpha = roughness*roughness;

	vec3 H = normalize(V+L);

	float dotNL = clamp(dot(N,L),0.0f,1.0f);

	float dotLH = clamp(dot(L,H),0.0f,1.0f);
	float dotNH = clamp(dot(N,H),0.0f,1.0f);

	float F, D, vis;

	// D
	float alphaSqr = alpha*alpha;
	float pi = 3.14159f;
	float denom = dotNH * dotNH *(alphaSqr-1.0) + 1.0f;
	D = alphaSqr/(pi * denom * denom);

	// F
	float dotLH5 = pow(1.0f-dotLH,5);
	F = F0 + (1.0-F0)*(dotLH5);

	// V
		float k = alpha/2.0f;
	vis = G1V(dotLH,k)*G1V(dotLH,k);

	float specular = dotNL * D * F * vis;
	return specular;
}



vec3 rgbToLinSpace(vec3 c) {

return vec3(pow(c.x,2.2) , pow(c.y,2.2) , pow(c.z,2.2) );
}
vec3 restore_pos_from_depth(float z,vec2 uv,mat4 proj, mat4 view) {
	mat4 inv_p = inverse(proj*view);
	vec4 s_pos = vec4(uv*2.0-1.0 ,z,1.0 );
	s_pos = inv_p * s_pos;
	return (s_pos.xyz/ s_pos.w);

}
float c_d(float a, float b) {
	if (a > b) 
		return 1.0;
	else
		return 0.0;
}
mat3 cotangent_frame( vec3 N, vec3 p, vec2 uv )
{
    // get edge vectors of the pixel triangle
    vec3 dp1 = dFdx( p );
    vec3 dp2 = dFdy( p );
    vec2 duv1 = dFdx( uv );
    vec2 duv2 = dFdy( uv );
 
    // solve the linear system
    vec3 dp2perp = cross( dp2, N );
    vec3 dp1perp = cross( N, dp1 );
    vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;
 
    // construct a scale-invariant frame 
    float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );
    return mat3( T * invmax, B * invmax, N );
}
float shadow_test(vec2 uv , float z) {
 return step(texture(sm_depth_sampler,uv).r,z - 0.000005);

}
void main() 
{

vec2 poissonDisk[64];
poissonDisk[0] = vec2(-0.613392, 0.617481);
poissonDisk[1] = vec2(0.170019, -0.040254);
poissonDisk[2] = vec2(-0.299417, 0.791925);
poissonDisk[3] = vec2(0.645680, 0.493210);
poissonDisk[4] = vec2(-0.651784, 0.717887);
poissonDisk[5] = vec2(0.421003, 0.027070);
poissonDisk[6] = vec2(-0.817194, -0.271096);
poissonDisk[7] = vec2(-0.705374, -0.668203);
poissonDisk[8] = vec2(0.977050, -0.108615);
poissonDisk[9] = vec2(0.063326, 0.142369);
poissonDisk[10] = vec2(0.203528, 0.214331);
poissonDisk[11] = vec2(-0.667531, 0.326090);
poissonDisk[12] = vec2(-0.098422, -0.295755);
poissonDisk[13] = vec2(-0.885922, 0.215369);
poissonDisk[14] = vec2(0.566637, 0.605213);
poissonDisk[15] = vec2(0.039766, -0.396100);
poissonDisk[16] = vec2(0.751946, 0.453352);
poissonDisk[17] = vec2(0.078707, -0.715323);
poissonDisk[18] = vec2(-0.075838, -0.529344);
poissonDisk[19] = vec2(0.724479, -0.580798);
poissonDisk[20] = vec2(0.222999, -0.215125);
poissonDisk[21] = vec2(-0.467574, -0.405438);
poissonDisk[22] = vec2(-0.248268, -0.814753);
poissonDisk[23] = vec2(0.354411, -0.887570);
poissonDisk[24] = vec2(0.175817, 0.382366);
poissonDisk[25] = vec2(0.487472, -0.063082);
poissonDisk[26] = vec2(-0.084078, 0.898312);
poissonDisk[27] = vec2(0.488876, -0.783441);
poissonDisk[28] = vec2(0.470016, 0.217933);
poissonDisk[29] = vec2(-0.696890, -0.549791);
poissonDisk[30] = vec2(-0.149693, 0.605762);
poissonDisk[31] = vec2(0.034211, 0.979980);
poissonDisk[32] = vec2(0.503098, -0.308878);
poissonDisk[33] = vec2(-0.016205, -0.872921);
poissonDisk[34] = vec2(0.385784, -0.393902);
poissonDisk[35] = vec2(-0.146886, -0.859249);
poissonDisk[36] = vec2(0.643361, 0.164098);
poissonDisk[37] = vec2(0.634388, -0.049471);
poissonDisk[38] = vec2(-0.688894, 0.007843);
poissonDisk[39] = vec2(0.464034, -0.188818);
poissonDisk[40] = vec2(-0.440840, 0.137486);
poissonDisk[41] = vec2(0.364483, 0.511704);
poissonDisk[42] = vec2(0.034028, 0.325968);
poissonDisk[43] = vec2(0.099094, -0.308023);
poissonDisk[44] = vec2(0.693960, -0.366253);
poissonDisk[45] = vec2(0.678884, -0.204688);
poissonDisk[46] = vec2(0.001801, 0.780328);
poissonDisk[47] = vec2(0.145177, -0.898984);
poissonDisk[48] = vec2(0.062655, -0.611866);
poissonDisk[49] = vec2(0.315226, -0.604297);
poissonDisk[50] = vec2(-0.780145, 0.486251);
poissonDisk[51] = vec2(-0.371868, 0.882138);
poissonDisk[52] = vec2(0.200476, 0.494430);
poissonDisk[53] = vec2(-0.494552, -0.711051);
poissonDisk[54] = vec2(0.612476, 0.705252);
poissonDisk[55] = vec2(-0.578845, -0.768792);
poissonDisk[56] = vec2(-0.772454, -0.090976);
poissonDisk[57] = vec2(0.504440, 0.372295);
poissonDisk[58] = vec2(0.155736, 0.065157);
poissonDisk[59] = vec2(0.391522, 0.849605);
poissonDisk[60] = vec2(-0.620106, -0.328104);
poissonDisk[61] = vec2(0.789239, -0.419965);
poissonDisk[62] = vec2(-0.545396, 0.538133);
poissonDisk[63] = vec2(-0.178564, -0.596057);
	float texture_alpha = texture(texture_alpha_sampler,uv).r;
	//fix me (get viewport sizes)
	vec4 sm_pos = sm_mat*vec4(o_pos_v ,1.0);
	sm_pos.xyz = sm_pos.xyz / sm_pos.w; 
	const int sm_samples = 64;

	float shadow = 0.0;

	//if (length(sm_pos.xyz) < 1.0 && dot(sm_pos.xyz, vec3())  ) {
	if (sm_pos.x > 0 && sm_pos.x < 1.0 && sm_pos.y > 0 && sm_pos.y < 1.0 && sm_pos.w > 0.0) {
		for (int i=0;i<sm_samples;i++)
		{
			shadow  += shadow_test(sm_pos.xy+poissonDisk[i]/shadowPenumbra,sm_pos.z);	

		}
		shadow = shadow / sm_samples;
	}
	else
		shadow = 1.0;
	shadow -=ambientIntensity ;
	const int rsm_samples = 3;
	/*rsm*/

	vec3 rsm_result = vec3(0.0);
	if (sm_pos.x > 0 && sm_pos.x < 1.0 && sm_pos.y > 0 && sm_pos.y < 1.0 && sm_pos.w > 0.0) {
	for (int i=0; i < rsm_samples;i++) {
		for (int j=0; j < rsm_samples;j++) {
		vec2 rsm_texcoord = vec2((float(i)/float(rsm_samples)),(float(j)/float(rsm_samples)));//poissonDisk[i]*0.5+0.5;
		vec3 rsm_pos = texture(rsm_vector_sampler,rsm_texcoord).xyz;
		vec3 rsm_normal =texture(rsm_normal_sampler,rsm_texcoord).xyz;
		vec3 rsm_albedo = texture(rsm_albedo_sampler,rsm_texcoord).xyz;
		float rsm_z = texture(rsm_normal_sampler,rsm_texcoord).w;

		float dist = 1.0f -  (min(1.0,length(rsm_pos - o_pos_v)/200 ));
		float flux2 = max(0.0 , dot(rsm_normal,normalize(rsm_pos-o_pos_v )))*max(0.0,dot(o_normal,normalize(o_pos_v -rsm_pos)));
		//float dist_orig = pow(length(rsm_pos - o_pos_v),1)/2000;
		//float est_orig = flux_orig/(dist_orig+0.00001);
		float l2_a = max(0.0, min( 1.0, dot( normalize(o_normal), normalize(rsm_pos-o_pos_v) ))+ 0.4);
      	float l2_b = max(0.0, dot(normalize(o_normal), normalize(rsm_normal)));
      	float flux = min(1.0, l2_a*l2_b);

		rsm_result += rsm_albedo*vec3(dist*flux2);
		//rsm_result  += rsm_albedo*vec3(est_orig);
	}
	}
	}
	//;rsm_result = rsm_result / (rsm_samples) ;//*rsm_samples);

	/* diffuse color texture*/
	
	vec3 texBump = normalize(2.0*(texture2D( texBUMP,uv).rgb)-1.0);
	
	texBump.z = sqrt( 1. - dot( texBump.xy, texBump.xy ) );

	//float texBump = texture2D( texBUMP,uv).r;


	vec3 d_normal;
	/*for model without normal we can try estimate it*/
	if (mesh_flags == MESH_V_TC) {
		vec3 X = dFdx(vPos);
		vec3 Y = dFdy(vPos);
		d_normal=normalize(cross(X,Y));
	} else {
		d_normal = t_normal; 
	}
	/* tangant basis matrix*/
	mat3 TBN =  cotangent_frame(d_normal,normalize(vPos),uv);

	//normal mapping
	d_normal = TBN * texBump;

	vec3 n = normalize(d_normal);
	vec3 v  = normalize(-vPos);
	vec3 l  = normalize( lightPos - vPos);
	vec3 h = normalize(v+l);
	/*amount of bump*/
	//texBump = (texBump - 0.5  ) /bump_amount; 
	/*tangant space view vector*/
	//vec3 tng_v = inverse(TBN) *normalize(-vPos);

	//vec2 parallax_uv = uv+texBump*tng_v.xy;
	//vec3 texColor = (texture2D( texIMG,parallax_uv).rgb);
	vec3 texColor = (texture2D( texIMG,uv).rgb);
	vec3 diffColor = texColor;//vec3(1.0f,0.71f,0.29f);
	vec3 specColor =  vec3(1.0);//vec3(1.0f,0.71f,0.29f);


	float diff = lambert (n,l);
	vec3 spec ;
	vec3 fressnel ;
	//spec = LightingFuncGGX_OPT2(n,v,l, 0.4,0.25);
	spec = binn_phong(n,h,l,2.0,diffColor,specColor)*lightIntensity*10;
	//spec = 0;
	fressnel =  F_schlick(l,h,spec*specColor);
	if (dot(n,l)< 0.0) 
		fressnel = vec3(0.0);
	float dist = length(vPos - lightPos );
	float att = 1.0 / (1.0 + 0.0001*dist + 0.0000001*dist*dist);
	//fressnel = vec3(0,0,0);
	//normal
	//dstColor = vec4(d_normal*0.5+0.5,1.0);
	//diff only 
	//dstColor = vec4(vec3(diff),1.0);
	//texture only 
	//dstColor = vec4(texColor,1.0);
	//spec only
	//dstColor = vec4(vec3(t_normal),1.0);
	//fresnell only
	//dstColor = vec4(vec3(fressnel)+0.05*diff,1.0);
	// spec+ diff+fressnel;
	//list attenuation
	//dstColor = vec4(vec3(att),1.0);
	//vec4 linColor = vec4(rsm_result*diffColor,1.0);
	//dstColor = vec4(pow(linColor.r,1/2.2),pow(linColor.g,1/2.2),pow(linColor.b,1/2.2),1.0);
	vec4 linColor = vec4((1.0-shadow)*(spec*att+5.0*(rsm_result*rsm_result)),texture_alpha);
	dstColor = linColor;


	worldNormal = vec4(0.5*normalize(t_normal)+0.5,texture_alpha);
}