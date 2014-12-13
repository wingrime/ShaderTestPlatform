#version 330
uniform vec4 vp; /* viewport conf*/
layout(location = 0) out vec4 color;
uniform sampler2D texSRC1;
uniform sampler2D texSRC2;

#define texDEPTH texSRC2
#define texFB texSRC1


uniform float b_dist2 = 0;//0.005;
float rand(vec2 co) {
	return fract(sin(dot(co.xy,vec2(12.9898, 78.233))) * 43758.5453);
}
float texSZ = 0.001;


vec3  normal_from_depth( vec2 c) {
	float d  = texture(texDEPTH, c).r;
	float dx = texture(texDEPTH, c + vec2( 0.0, 0.0005) ).r;
	float dy = texture(texDEPTH, c + vec2(0.0005 ,0.0) ).r;
	//float dxn = texture(texDEPTH, c + vec2( 0.0, -0.0005) ).r;
	//float dyn = texture(texDEPTH, c + vec2(-0.0005 ,0.0) ).r;

	vec3 v1 = vec3(0.0005 , 0.0 ,  dx-d);
	vec3 v2 = vec3(0.0, 0.0005  ,  dy-d);
	vec3 n = cross(v1,v2);
	//n.z = -n.z;
	return normalize(n);
	
	


}
uniform float ssaoSize  =0.01;
const int Samples = 20;
void main ()
{
  
	vec2 c = gl_FragCoord.xy/vp.xy;
	//vec3 n = normal_from_depth(c);
	vec3 n =  normalize(texture(texFB,c).xyz*2.0-1.0);
	float depth = texture(texDEPTH,c).r;
	
	float R = ssaoSize/depth;
	
	vec3 Rnd[Samples];
	//gaussian dist
	Rnd[0] = vec3((rand(c)),(rand(n.xy-c)),(rand(n.yx-c+vec2(0.2,0.2))));
	
	for (int i = 1; i < Samples; i++) {
	
	Rnd[i] = vec3((rand(Rnd[i-1].xy+vec2(0.6,0.1))-0.5)*2,(rand(Rnd[i-1].xy)-0.5)*2, (rand(Rnd[i-1].yx)-0.5)*2 );
	}

	float occ_depthf = 0;
	for (int i = 0; i< Samples; i++) {

		vec3 hemi = sign(dot(Rnd[i],n))*Rnd[i].xyz;	
		if (dot (hemi, n) > 0.20) // clamp HI angles
		{
		float occ_depth = texture(texDEPTH,c+(R*hemi.xy)).r;
		float diff = depth  - occ_depth;
		if (diff > 0 && diff < 0.02 )
			occ_depthf +=1;
		}
	}

	
	occ_depthf = clamp((occ_depthf / Samples) - 0.5,0,1);	

	
	//TURN ON SSAO
	//color = texture(texFB,c)*(1-occ_depthf);
	//IMAGE ONLY
	//color = texture(texFB,c);
	//SSAO ONLY
	color = vec4 (vec3(1-occ_depthf),1.0);
	//DEPTH TO NORMAL TEST
	//color = vec4(n,1.0);
}