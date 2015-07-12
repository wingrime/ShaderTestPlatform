#version 330
/* Base Lighting Shader */
layout(location = 0) out vec4 dstColor;
const float pi = 3.14159;
uniform vec4 main_light_dir = vec4(0.0,1.0,0.0,0.0);

uniform mat4 cam_proj;
uniform mat4 model;
uniform mat4 view;

uniform mat4 MVP;
uniform mat4 MV;

uniform mat4 matrixShadowMapProjection;
uniform mat4 matrixShadowMapView ;

/* BRDF Required vectors*/

flat in vec3 LightCS;
in vec3 PositionCS;
in vec2 UvMS;

in vec3 NormalCS;
in vec3 NormalMS;
in vec3 PositionMS;

/* Source texture */
uniform sampler2D samplerAlbedo;
uniform sampler2D samplerNormalMap;
uniform sampler2D samplerAlphaMap;
uniform sampler2D samplerEnvSH;
uniform sampler2D samplerRandomNoise;
/* Shadow map depth buffer*/
uniform sampler2DArray samplerShadowMap;
/*test*/
uniform float slice= 0.0;
uniform samplerCube samplerEnvCubeMap;

uniform float lightIntensity = 1.0;
uniform float shadowPenumbra = 200.0;
uniform float shadowEps =   0.0001;
uniform float shadowEpsMult =   0.0001;
uniform	float s0 = 290.0;
uniform	float s1 = 840.0;
uniform	float s2 = 2420.0;
uniform	float s3 = 7000.0;
uniform float ambientIntensity = 0.1;
uniform float sh_int = 5.2;
uniform float ggx_alpha = 0.4;
uniform float ggx_f0 = 0.04 ;/*fressnel*/
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
uniform mat4 shadowMVPB0;
uniform mat4 shadowMVPB1;
uniform mat4 shadowMVPB2;
uniform mat4 shadowMVPB3;
uniform float kf = 1300.0;

/*fressnel aprox*/
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
vec3 ggx_full_t(vec3 n, vec3 l , vec3 h, vec3 v, float alpha,float f0,vec3 c_diff,vec3 c_spec ) {

	return c_diff*(clamp(dot(n,l),0.0,1.0))+LightingFuncGGX_OPT2(n,v,l,alpha,f0);
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
float svs(float s) {
	return ((s-100.0) / (7000.0-100.0)); 
}

//Box Projected Cube Environment Mapping

vec3 bpcem (vec3 d, vec3 Emax, vec3 Emin, vec3 Epos, vec3 Pos)

{

    // All calculations in worldspace

    vec3 nrdir = normalize(d);

    vec3 rbmax = (Emax - Pos)/nrdir;
    vec3 rbmin = (Emin - Pos)/nrdir;

    vec3 rbminmax;

    float Distance = min(min(rbminmax.x, rbminmax.y), rbminmax.z);

    vec3 posonbox = Pos + nrdir * Distance;

    return posonbox - Epos;

}

float LinearizeDepth(float depthNDC)
{
  float n = 100.0; // camera z near
  float f = 7000.0; // camera z far
  return (2.0 * n) / (f + n - depthNDC * (f - n));	
}

void main() 
{

vec2 poissonDisk[64];
poissonDisk[0] = vec2(-0.613392, 0.617481); poissonDisk[1] = vec2(0.170019, -0.040254);
poissonDisk[2] = vec2(-0.299417, 0.791925); poissonDisk[3] = vec2(0.645680, 0.493210);
poissonDisk[4] = vec2(-0.651784, 0.717887); poissonDisk[5] = vec2(0.421003, 0.027070);
poissonDisk[6] = vec2(-0.817194, -0.271096);poissonDisk[7] = vec2(-0.705374, -0.668203);
poissonDisk[8] = vec2(0.977050, -0.108615); poissonDisk[9] = vec2(0.063326, 0.142369);
poissonDisk[10] = vec2(0.203528, 0.214331); poissonDisk[11] = vec2(-0.667531, 0.326090);
poissonDisk[12] = vec2(-0.098422, -0.295755); poissonDisk[13] = vec2(-0.885922, 0.215369);
poissonDisk[14] = vec2(0.566637, 0.605213); poissonDisk[15] = vec2(0.039766, -0.396100);
poissonDisk[16] = vec2(0.751946, 0.453352); poissonDisk[17] = vec2(0.078707, -0.715323);
poissonDisk[18] = vec2(-0.075838, -0.529344); poissonDisk[19] = vec2(0.724479, -0.580798);
poissonDisk[20] = vec2(0.222999, -0.215125); poissonDisk[21] = vec2(-0.467574, -0.405438);
poissonDisk[22] = vec2(-0.248268, -0.814753); poissonDisk[23] = vec2(0.354411, -0.887570);
poissonDisk[24] = vec2(0.175817, 0.382366); poissonDisk[25] = vec2(0.487472, -0.063082);
poissonDisk[26] = vec2(-0.084078, 0.898312);poissonDisk[27] = vec2(0.488876, -0.783441);
poissonDisk[28] = vec2(0.470016, 0.217933); poissonDisk[29] = vec2(-0.696890, -0.549791);
poissonDisk[30] = vec2(-0.149693, 0.605762); poissonDisk[31] = vec2(0.034211, 0.979980);
poissonDisk[32] = vec2(0.503098, -0.308878); poissonDisk[33] = vec2(-0.016205, -0.872921);
poissonDisk[34] = vec2(0.385784, -0.393902); poissonDisk[35] = vec2(-0.146886, -0.859249);
poissonDisk[36] = vec2(0.643361, 0.164098); poissonDisk[37] = vec2(0.634388, -0.049471);
poissonDisk[38] = vec2(-0.688894, 0.007843); poissonDisk[39] = vec2(0.464034, -0.188818);
poissonDisk[40] = vec2(-0.440840, 0.137486); poissonDisk[41] = vec2(0.364483, 0.511704);
poissonDisk[42] = vec2(0.034028, 0.325968); poissonDisk[43] = vec2(0.099094, -0.308023);
poissonDisk[44] = vec2(0.693960, -0.366253); poissonDisk[45] = vec2(0.678884, -0.204688);
poissonDisk[46] = vec2(0.001801, 0.780328); poissonDisk[47] = vec2(0.145177, -0.898984);
poissonDisk[48] = vec2(0.062655, -0.611866); poissonDisk[49] = vec2(0.315226, -0.604297);
poissonDisk[50] = vec2(-0.780145, 0.486251); poissonDisk[51] = vec2(-0.371868, 0.882138);
poissonDisk[52] = vec2(0.200476, 0.494430); poissonDisk[53] = vec2(-0.494552, -0.711051);
poissonDisk[54] = vec2(0.612476, 0.705252); poissonDisk[55] = vec2(-0.578845, -0.768792);
poissonDisk[56] = vec2(-0.772454, -0.090976); poissonDisk[57] = vec2(0.504440, 0.372295);
poissonDisk[58] = vec2(0.155736, 0.065157); poissonDisk[59] = vec2(0.391522, 0.849605);
poissonDisk[60] = vec2(-0.620106, -0.328104); poissonDisk[61] = vec2(0.789239, -0.419965);
poissonDisk[62] = vec2(-0.545396, 0.538133); poissonDisk[63] = vec2(-0.178564, -0.596057);

        float texture_alpha = texture(samplerAlphaMap,UvMS).r;
	//fix me (get viewport sizes)
	vec3 tesc;
	const int sm_samples = 32;
	float slice_sel = 0;
	float far = 7000;
	float near = 100;
	float shadow;
	vec4 sm_pos;
	//not general way for light with dir 0,1,0
	//normal way length(light_dir - o_pos_v)
	/*slowest sol*/
        vec4 view_space = inverse(cam_proj)*gl_FragCoord;
	view_space.xyz /= view_space.w; // ?

	float d =  1.0-view_space.z;//((2*far*near) /((2.0*gl_FragCoord.z-1.0)*(far-near)-(far+near)));// far-near;
	//float  d = gl_FragCoord
	if (d < svs(s0) )
	{
		slice_sel = 0;
		 tesc = vec3(1.0,0.0,0.0);
                  sm_pos = shadowMVPB0*vec4(PositionMS ,1.0);
	} else if (d < svs(s1)) {
		slice_sel = 1;
		tesc = vec3(1.0,1.0,0.0);
                sm_pos = shadowMVPB1*vec4(PositionMS ,1.0);
	} else if (d < svs(s2)) {
		slice_sel = 2;
		tesc = vec3(0.0,1.0,0.0);
                sm_pos = shadowMVPB2*vec4(PositionMS ,1.0);
	}
	else if (d < svs(s3)) {
		slice_sel = 3;
		tesc = vec3(0.0,0.0,1.0);
                sm_pos = shadowMVPB3*vec4(PositionMS ,1.0);
	} else {
		tesc = vec3(1.0,1.0,1.0);
		slice_sel = 3;
                sm_pos = shadowMVPB3*vec4(PositionMS ,1.0);

	}
    //vec4 sm_pos = shadowMVPB*vec4(o_pos_v+o_pos_v*(normalize(o_normal)*shadowEps) ,1.0);
sm_pos = shadowMVPB2*vec4(PositionMS ,1.0);


   
    /* PCF soft shadows with posion sampling */
// HARD shadows
//shadow  =  1.0-step(texture(sm_depth_sampler,vec3(sm_pos.xy,float(1))).r,sm_pos.z);
//shadow  =texture(sm_depth_sampler,vec4(sm_pos.xyz,float(1)));


	/* diffuse color texture*/
	
        vec3 texB = 2.0*(texture( samplerNormalMap,UvMS).rgb)-1.0;
	
	texB.z = sqrt( 1. - dot( texB.xy, texB.xy ) );

	vec3 texBump = normalize(texB);

	//float texBump = texture2D( texBUMP,uv).r;


	

	/* tangant basis matrix*/
        mat3 TBN =  cotangent_frame(normalize(NormalCS),PositionCS,UvMS);

	//normal mapping
	vec3 d_normal  = TBN * texBump;

/*
Directional Light not translable, so set w to 0.0;
*/
	vec4 sunDirectionalLightWS = vec4(main_light_dir.xyz,0.0);
    vec3 lp = (view*sunDirectionalLightWS).xyz;


	vec3 n = normalize(d_normal);
    vec3 v  = normalize(-PositionCS);
    // vec3 l  = normalize( lp - PositionCS); /*point light */
	vec3 l = normalize (lp.xyz); /* directional light */
	vec3 h = normalize(v+l);

	/*amount of bump*/
	//texBump = (texBump - 0.5  ) /bump_amount; 
	/*tangant space view vector*/
	//vec3 tng_v = inverse(TBN) *normalize(-vPos);

	//vec2 parallax_uv = uv+texBump*tng_v.xy;
	//vec3 texColor = (texture2D( texIMG,parallax_uv).rgb);
    vec3 texColor = (texture( samplerAlbedo,UvMS).rgb);
	vec3 diffColor = texColor;//vec3(1.0f,0.71f,0.29f);
	vec3 specColor =  vec3(1.0);//vec3(1.0f,0.71f,0.29f);
	float diff = lambert (n,l);


  /* PCF soft shadows with possion sampling */
   	sm_pos.xyz /= sm_pos.w;
    sm_pos.z -= shadowEps ;
	//Automatic biasing v1
	//sm_pos.z -= clamp(shadowEps*tan(acos (diff)) ,0,0.1) ;

	shadow = 0.0;
 	float shadowPenumbraDisp = shadowPenumbra ;
 	/*Too costly !!!*/
 	vec2 randomFromTexture = normalize(-1.0+2.0*texture(samplerRandomNoise, 0.5+0.5*reflect(UvMS,gl_FragCoord.yx)).gb);
if (  clamp(sm_pos.xyz,1.0,0.0) != sm_pos.xyz) {
                vec4 s;
                for (int i=0;i<sm_samples;i+=4)
                {
                    s.x = step(texture(samplerShadowMap,vec3(sm_pos.xy+vec2(reflect(poissonDisk[i],randomFromTexture)/shadowPenumbraDisp),slice)).r,sm_pos.z);
                   	s.y = step(texture(samplerShadowMap,vec3(sm_pos.xy+vec2(reflect(poissonDisk[i+1],randomFromTexture)/shadowPenumbraDisp),slice)).r,sm_pos.z);
                   	s.z = step(texture(samplerShadowMap,vec3(sm_pos.xy+vec2(reflect(poissonDisk[i+2],randomFromTexture)/shadowPenumbraDisp),slice)).r,sm_pos.z);
                   	s.w = step(texture(samplerShadowMap,vec3(sm_pos.xy+vec2(reflect(poissonDisk[i+3],randomFromTexture)/shadowPenumbraDisp),slice)).r,sm_pos.z); 
                    shadow += dot (s,vec4(1.0));

                }
                shadow = 1.0- ( (1.0 / sm_samples) * shadow);
        }
        else
                shadow = 0.0;
       
	// in case when surface oriented oposite to lightdir are in dark
	// optimization

// HARD shadows
//shadow  =  1.0-step(texture(sm_depth_sampler,vec3(sm_pos.xy,slice)).r,sm_pos.z);
//shadow  =texture(sm_depth_sampler,vec4(sm_pos.xyz,float(1)));
// shadow =  clamp(shadow,0.0,1.0);//bug check
	if (dot(n,l) <= 0) {
		shadow = 0.0;
		//diffColor = vec3(1.0,0.0,0.0);
	}

	
	
	//spec = LightingFuncGGX_OPT2(n,v,l, 0.4,0.25)*lightIntensity*F_schlick(h,l,vec3(diffColor))*diff;
	//spec = binn_phong(n,h,l,2.0,diffColor,specColor)*lightIntensity*10;
	vec3 spec =ggx_full_t(n,h,l,v,ggx_alpha,ggx_f0,diffColor,specColor)*lightIntensity;
	//spec = 0;
	vec3 fressnel =  F_schlick(l,n,diff*vec3(0.003,0.003,0.003));
	//float dist = length(vPos - lightPos );
	//float att = 1.0 / (1.0 + 0.0001*dist + 0.0000001*dist*dist);
	//useless


	/*SH bands load*/
	vec3  SH[9];
	for (int c = 0 ; c < 9; c++) {
		SH[c].r = texelFetch(samplerEnvSH, ivec2(c,1u),0).r;
		SH[c].g = texelFetch(samplerEnvSH, ivec2(c,2u),0).r;
		SH[c].b = texelFetch(samplerEnvSH, ivec2(c,3u),0).r;
        }
        //vec3 ambient_spectral_harmonics = appplySHamonics(SH,(transpose(MV_n)*vec4(normalize(d_normal),1.0)).xyz);
                vec3 ambient_spectral_harmonics = appplySHamonics(SH,(transpose(MV)*vec4(normalize(reflect(v,n)),1.0) ).xyz);



	/*reflections */
        vec3 cubemap_sample = vec3(texture(samplerEnvCubeMap,(transpose(MV)*vec4(normalize(reflect(v,n)),1.0) ).xyz).xyz);

	vec3 reflection = fressnel*cubemap_sample;

	if (dbg_out == DBG_OUT_FULL)
		dstColor = vec4((shadow)*(spec)+(ambient_spectral_harmonics)*sh_int*diffColor,texture_alpha);
	else if (dbg_out ==  DBG_OUT_ALBEDO)
		dstColor = dstColor = vec4(vec3(diff),1.0);
	else if (dbg_out ==  DBG_OUT_DIFFUSE)
		dstColor =  vec4(diff*diffColor,1.0);
	else if (dbg_out ==  DBG_OUT_ALPHA)
		dstColor =  vec4(vec3(0.0,texture_alpha,0.0),1.0);
	else if (dbg_out ==  DBG_OUT_DIFFUSE)
		dstColor =  vec4(vec3(0.0,texture_alpha,0.0),1.0);	
	else if (dbg_out ==  DBG_OUT_NORMAL)
                dstColor = vec4(0.5*normalize(NormalCS)+0.5,1.0);
	else if (dbg_out ==  DBG_OUT_ATTEN)
		dstColor =  vec4(vec3(0.0,1.0,0.0),1.0);	
	else if (dbg_out ==  DBG_OUT_FRESSNEL)
		dstColor =  vec4(fressnel,1.0);	
	else if (dbg_out ==  DBG_OUT_NORMAL_MAPED)
		dstColor = vec4(0.5*normalize(d_normal)+0.5,1.0);
	else if (dbg_out ==  DBG_OUT_SHADOW)
		dstColor = vec4(vec3(tesc),1.0);
        else if (dbg_out ==  DBG_OUT_REFLECT) {

            //dstColor = vec4(vec3(texture(samplerEnvCubeMap,(transpose(_n)*vec4(normalize(reflect(v,n)),1.0) ).xyz).xyz),1.0);

            vec3 relfectWS = (transpose(view)*vec4(normalize(reflect(v,n)),1.0) ).xyz; /*cubemap placed at 0, and we can use v for it*/
            /*cheap approximation https://seblagarde.files.wordpress.com/2012/08/parallax_corrected_cubemap-gameconnection2012.pdf */
            //vec3 bpcem (vec3 v, vec3 Emax, vec3 Emin, vec3 Epos, vec3 Pos)
            vec3 AABBmin= vec3(-1200,0,-200.0);
            vec3 AABBmax = vec3(1200,1200,200.0);
            vec3 CubemapPos = vec3 (0,500,0);
            /*BPCEM*/
            vec3 nrdir = normalize (relfectWS);
            vec3 rbmax = AABBmax/nrdir;            // AABB max value +...
            vec3 rbmin = AABBmin/nrdir;                          // AABB min value +...
            vec3 rbminmax;
            rbminmax.x = (nrdir.x>0.0)?rbmax.x:rbmin.x;
            rbminmax.y = (nrdir.y>0.0)?rbmax.y:rbmin.y;
            rbminmax.z = (nrdir.z>0.0)?rbmax.z:rbmin.z;                          // ...?

            float fa = min(min(rbminmax.x, rbminmax.y), rbminmax.z);                    // ...?
            vec3 posonbox = PositionMS + nrdir*fa;                                   // ...?
            vec3 rdir = posonbox -CubemapPos ;



            dstColor = vec4(diffColor*diff,1.0)*0.5+0.5*vec4(vec3(texture(samplerEnvCubeMap,rdir).xyz),1.0);
            }
            else if (dbg_out ==  DBG_OUT_SH)
		dstColor = vec4(vec3(ambient_spectral_harmonics),1.0);
	else if (dbg_out ==  DBG_OUT_GAMMA) {
			vec4 linColor = vec4((1.0-shadow)*(spec),texture_alpha);
                        dstColor = vec4(pow(linColor.r,1/2.2),pow(linColor.g,1/2.2),pow(linColor.b,1/2.2),1.0);
		}
	else 
		dstColor = vec4(1.0);

		/*nan protetion*/
	if (any(isnan(dstColor)))
		dstColor=  vec4(vec3(0.0),1.0);

		//if (slice_sel == 0)
		//	dstColor=  vec4(vec3(0.0,1.0,0.0),1.0);
		//	else if (slice_sel == 1)
		//	dstColor=  vec4(vec3(1.0,1.0,0.0),1.0);
		//	else if (slice_sel == 2)
		//	dstColor=  vec4(vec3(1.0,0.0,0.0),1.0);
		//	else 
		//	dstColor=  vec4(vec3(1.0,0.0,1.0),1.0);
}
