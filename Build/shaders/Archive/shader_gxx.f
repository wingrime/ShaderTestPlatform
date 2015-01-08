/* Base Lighting Shader */
#version 330
out vec4 color;
in vec4 n;
in vec4 p;

in vec2 uv;
in vec4 lp;

uniform sampler2D tex_sampler;

vec3 diff_color = vec3(0.0f,0.0f,1.0f);
float s =10.0f;
float light_atten = 0.001f;

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

void main() 
{
vec3 tn_tangentspace = normalize((texture(tex_sampler,uv).rgb)*2.0 - 1.0);
vec3 tex_color = texture(tex_sampler,uv).rgb;
vec3 diffcolor = vec3(1.0f,0.5f,0.0f);
vec3 speccolor = vec3(1.0f,0.5f,0.0f);


	float diff = max(0.0f,dot(normalize(n),normalize(lp-p)));

	//float spec = pow( max(0.0f , dot(normalize(n.xyz),normalize((-p.xyz) + normalize(lp.xyz-p.xyz) )) ), s);
	float spec = max(0.0f,LightingFuncGGX_OPT2(n.xyz,p.xyz,lp.xyz,0.02f,0.001f));
	
	float distanceToLight = length(lp - p);
    float attenuation = 1.0- light_atten * pow(distanceToLight, 2);
    //fullscreen 
	//vec3 tex_color = texture(tex_sampler,gl_FragCoord.xy/vec2(1000.0f,1000.0f)).rgb;//vec4(texture ( tex_sampler, UV));

	//color = vec4(vec3(mix(diff,spec,0.5)),1.0);
	//color = vec4(vec3(n.xyz),1.0);
	//color = vec4(vec3(diff*0.5),1.0);
	//color = vec4((vec3(tex_color*(diff)+spec)*attenuation) ,1.0);
	color = vec4(  (tex_color*vec3(diff+spec)*attenuation) ,1.0);
	//color = vec4(tex_color,1.0);
	//color = vec4(dot(n.xyz,lp.xyz),1.0);
}