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
/* Source texture */
uniform float bump_amount = 50.0;
uniform sampler2D texIMG;
uniform sampler2D texBUMP;
uniform sampler2D texture_alpha_sampler;
uniform sampler2D sh_bands_sampler;
/* Shadow map depth buffer*/
uniform sampler2D sm_depth_sampler;

uniform sampler2D rsm_normal_sampler;
uniform samplerCube rsm_vector_sampler;
uniform sampler2D rsm_albedo_sampler;

uniform float lightIntensity = 1.0;
uniform float shadowPenumbra = 200.0;
uniform float ambientIntensity = 0.1;
uniform float sh_int = 10.2;
#define DBG_OUT_FULL 0
#define DBG_OUT_NORMAL  1
#define DBG_OUT_ALBEDO  2
#define DBG_OUT_NORMAL_ORIGINAL  3
#define DBG_OUT_DIFFUSE  4
#define DBG_OUT_ALPHA  5
#define DBG_OUT_ATTEN 6
#define DBG_OUT_FRESSNEL 7
#define DBG_OUT_NORMAL_MAPED 8 
#define DBG_OUT_SHADOW 9 
#define DBG_OUT_GAMMA 10
#define DBG_OUT_REFLECT 11
#define DBG_OUT_SH 12
uniform int dbg_out = 0;



vec3 F_schlick(vec3 l, vec3 n,vec3 c_spec) {

	return clamp(c_spec+ (vec3(1.0f)-c_spec)*pow(1.0- max(dot(l,n),0.000),5),0.0,1.00); 
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

float c_d(float a, float b) {
	if (a > b) 
		return 1.0;
	else
		return 0.0;
}
/*TODO: offline */
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

/*
vec3 appplySHamonics(vec3 L[9],vec3 n) {
	const vec3 c1 = vec3(0.429043);
	const vec3 c2 = vec3(0.511664);
	const vec3 c3 = vec3(0.743125);
	const vec3 c4 = vec3(0.886227);
	const vec3 c5 = vec3(0.247708);
	return c1*L[8]*(n.x*n.x - n.y*n.y ) + c3*L[6]*n.z*n.z +c4*L[0] - c5*L[6] +
		2*c1 *( L[4]*n.x*n.y  + L[7]*n.x*n.z + L[5]*n.y*n.z ) +
		2*c2 *( L[3]*n.x+ L[1]*n.y + L[2]*n.z );
}*/
vec3 appplySHamonics(vec3 L[9],vec3 n) {
	return vec3(0.282094791)*L[0] +  vec3(0.48860251)*(n.y*L[1]+n.z*L[2] + n.x*L[3] ) + 
		 vec3(1.092548430)*(n.x*n.y*L[4] +n.y*n.z*L[5]) + 
		 vec3(0.315391565)*(3.0*n.z*n.z-1.0)*L[6] +
		 vec3(0.772744)*n.z*n.x*L[7] +
		 vec3(0.386372)*(n.x*n.x-n.y*n.y)*L[8];
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

	/*SH bands load*/
	vec3  SH[9];
	for (int c = 0 ; c < 9; c++) {
		SH[c].r = texelFetch(sh_bands_sampler, ivec2(c,1u),0);
		SH[c].g = texelFetch(sh_bands_sampler, ivec2(c,2u),0);
		SH[c].b = texelFetch(sh_bands_sampler, ivec2(c,3u),0);
	}
	vec3 ambient_spectral_harmonics = appplySHamonics(SH,normalize(o_normal.xyz));

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
	//shadow -=ambientIntensity ;


	/* diffuse color texture*/
	
	vec3 texBump = normalize(2.0*(texture2D( texBUMP,uv).rgb)-1.0);
	
	texBump.z = sqrt( 1. - dot( texBump.xy, texBump.xy ) );

	//float texBump = texture2D( texBUMP,uv).r;


	vec3 d_normal;

	/* tangant basis matrix*/
	mat3 TBN =  cotangent_frame(t_normal,normalize(vPos),uv);

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
	//spec = LightingFuncGGX_OPT2(n,v,l, 0.4,0.25)*diffColor*diff*lightIntensity*10;
	spec = binn_phong(n,h,l,2.0,diffColor,specColor)*lightIntensity*10;
	//spec = 0;
	fressnel =  F_schlick(l,n,diff*vec3(0.003,0.003,0.003));
	if (dot(n,l)< 0.0) 
		fressnel = vec3(0.0);
	float dist = length(vPos - lightPos );
	float att = 1.0 / (1.0 + 0.0001*dist + 0.0000001*dist*dist);





	/*reflections */
	vec3 cubemap_sample = vec3(texture(rsm_vector_sampler,(transpose(MV_n)*vec4(normalize(reflect(v,n)),1.0) ).xyz).xyz);

	vec3 reflection = fressnel*cubemap_sample;

	if (dbg_out == DBG_OUT_FULL)
		dstColor = vec4((1.0-shadow)*(spec*att)+(ambient_spectral_harmonics)*sh_int*diffColor,texture_alpha);
	else if (dbg_out ==  DBG_OUT_ALBEDO)
		dstColor = dstColor = vec4(vec3(diff),1.0);
	else if (dbg_out ==  DBG_OUT_DIFFUSE)
		dstColor =  vec4(diff*diffColor,1.0);
	else if (dbg_out ==  DBG_OUT_ALPHA)
		dstColor =  vec4(vec3(0.0,texture_alpha,0.0),1.0);
	else if (dbg_out ==  DBG_OUT_DIFFUSE)
		dstColor =  vec4(vec3(0.0,texture_alpha,0.0),1.0);	
	else if (dbg_out ==  DBG_OUT_NORMAL)
		dstColor = vec4(0.5*normalize(t_normal)+0.5,1.0);
	else if (dbg_out ==  DBG_OUT_ATTEN)
		dstColor =  vec4(vec3(0.0,att,0.0),1.0);	
	else if (dbg_out ==  DBG_OUT_FRESSNEL)
		dstColor =  vec4(fressnel,1.0);	
	else if (dbg_out ==  DBG_OUT_NORMAL_MAPED)
		dstColor = vec4(0.5*normalize(d_normal)+0.5,1.0);
	else if (dbg_out ==  DBG_OUT_SHADOW)
		dstColor = vec4(vec3(ambient_spectral_harmonics),1.0);
	else if (dbg_out ==  DBG_OUT_REFLECT)
		dstColor = vec4(vec3(texture(rsm_vector_sampler,(transpose(MV_n)*vec4(normalize(reflect(v,n)),1.0) ).xyz).xyz),1.0);
	else if (dbg_out ==  DBG_OUT_SH)
		dstColor = vec4(vec3(ambient_spectral_harmonics),1.0);
	else if (dbg_out ==  DBG_OUT_GAMMA) {
			vec4 linColor = vec4((1.0-shadow)*(spec*att),texture_alpha);
			dstColor = vec4(pow(linColor.r,1/2.2),pow(linColor.g,1/2.2),pow(linColor.b,1/2.2),1.0);
		}
	else 
		dstColor = vec4(1.0);

	worldNormal = vec4(0.5*normalize(t_normal)+0.5,texture_alpha);
}