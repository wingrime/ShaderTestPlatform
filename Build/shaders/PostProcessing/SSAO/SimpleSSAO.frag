#version 330
uniform vec4 vp; /* viewport conf*/
layout(location = 0) out vec4 color;
uniform sampler2D texSRC1;
uniform sampler2D texSRC2;
uniform mat4 m_P;
uniform float ssaoLevelClamp;
#define texDEPTH texSRC2
#define texFB texSRC1
in vec2 uv;

uniform float b_dist2 = 0;//0.005;
uint hash (uint x) {
x+= x<<10u;
x^= x>>6u;
x+= x<<3u;
x^= x>>11u;
x+= x<<15u;

return x;
}

float floatConstruct(uint m) {
    const uint ieeeMantissa = 0x007fffffu; // mantissa bitmask
    const uint ieeeOne = 0x3f800000u; // 1.0 in uint

m &= ieeeMantissa;
m |= ieeeOne;

float f = uintBitsToFloat(m);
return f - 1.0;
}
uint seed;
float rnd() {
seed = hash(seed);
return floatConstruct(seed);

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
float LinearizeDepth(vec2 _uv)
{
  float n = 100.0; // camera z near
  float f = 7000.0; // camera z far
  float z = texture2D(texDEPTH, _uv).x;
  return (2.0 * n) / (f + n - z * (f - n));	
}
uniform float ssaoSize  =0.01;
const int Samples = 32;
void main ()
{


	vec2 c = uv;
	//vec3 n = normal_from_depth(c);
	vec3 n =  normalize(texture(texFB,c).xyz*2.0-1.0);
        seed = hash(hash(1u+hash(1u+ hash(1u+floatBitsToUint(n.x)))+hash(1u+hash(1u+floatBitsToUint(n.y+c.y))) +hash(1u+hash(1u+floatBitsToUint(n.z+c.x)) )));
        //..seed = hash(floatBitsToUint(n.x*n.y+n.z+c.x*c.y)));
	//float depth = texture(texDEPTH,c).r;
	// WHY NOT WORKS??
	//mat4 invP = inverse(m_P);
	//vec4 NDC = invP*vec4(2.0*uv-1.0,depth,1.0);

	//NDC.xyz /= NDC.w;

	float depth = LinearizeDepth(uv);//pow(NDC.z,64);;

	float R = (ssaoSize/50.0)/depth;
	

	float occ_depthf = 0;
	for (int i = 0; i< Samples; i++) {
		vec3 v_rnd = normalize(vec3(rnd()*2.0-1.0,rnd()*2.0-1.0,rnd()));
		v_rnd *= rnd();
		vec3 hemi = sign(dot(v_rnd,n))*v_rnd;	
		//if (dot (hemi, n) < 0.65 ) // clamp HI angles
		{
		float occ_depth = LinearizeDepth(uv+R*hemi.xy);
		float diff = depth  - occ_depth;
		if (diff > 0.001 )

			occ_depthf += min(1.0,R / abs(diff));
		}
	}

	
	occ_depthf = clamp(clamp(occ_depthf / Samples,0.0,1.0) - ssaoLevelClamp*1.0,0.0,1.0);	

	
	color = vec4 (vec3(1.0-occ_depthf),1.0);
    //DEPTH TO NORMAL TEST
	//color = vec4(n,1.0);
	//color = vec4(vec3(depth),1.0);
}
