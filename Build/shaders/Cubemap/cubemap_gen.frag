/* Base Lighting Shader */
#version 430 core
layout(location = 0) out vec4 dstColor;

const float pi = 3.14159;


in vec3 lightPos;
in vec3 vPos;
in vec2 uv;
in vec3 t_normal;





uniform sampler2D samplerAlbedo;
uniform sampler2D samplerAlphaMap;


uniform float lightIntensity = 1.0;
uniform float shadowPenumbra = 200.0;
uniform float ambientIntensity = 0.1;
/*lambertian cosine law with normalization factor*/
float lambert(vec3 n , vec3 l)  {
	return max(0.0f, dot(n,l));
}
void main() 
{


	//float texture_alpha = texture(samplerAlphaMap,uv).r;	

	vec3 n = normalize(t_normal);
	vec3 v  = normalize(-vPos);
	vec3 l  = normalize( lightPos - vPos);
	vec3 h = normalize(v+l);


	vec3 texColor = (texture2D( samplerAlbedo,uv).rgb);
	vec3 diffColor = texColor;//vec3(1.0f,0.71f,0.29f);
	vec3 specColor =  vec3(1.0);//vec3(1.0f,0.71f,0.29f);


	vec3 diff = lambert (n,l)*texColor;

	dstColor = vec4(vec3(diff),1.0);

	
}