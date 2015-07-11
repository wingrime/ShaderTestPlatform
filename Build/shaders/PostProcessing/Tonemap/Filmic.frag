#version 330
uniform vec4 vp; /* viewport conf*/
in vec2 uv;
layout(location = 0) out vec4 color;
uniform sampler2D texSRC1;
uniform sampler2D texSRC2;
uniform sampler2D texSRC3;
uniform sampler2D texSRC4;
#define texHDR texSRC2
#define texBLUM texSRC1
#define texSSAO texSRC4
#define texLumKey texSRC3
uniform float aoStrength = 1.0;
uniform float A = 0.22;
uniform float B = 0.30;
uniform float C = 0.10;
uniform float D = 0.20;
uniform float E = 0.01;
uniform float F = 0.30;
uniform float LW = 1.2;
vec3 filmic(vec3 x) {
  return ((x*(A*x+C*B)+D*E)/ (x*(A*x+B)+D*F))-E/F;
}

/* gamma correction */
float sRGB(float x) {
  if ( x <= 0.00031308)
    return 12.92*x;
  else
    return 1.055*pow(x,(1/2.4) ) - 0.055;
}

vec3 sRGB_v3(vec3 c) {
  return vec3(sRGB(c.x),sRGB(c.y),sRGB(c.z));
}
/*fast variant that ignores small things*/
vec3 sRGB_fast(vec3 x) {
    return 1.055*pow(x,vec3(1/2.4) ) - vec3(0.055);
}

vec3 gamma_v3( vec3 c) {
  return sRGB_v3(filmic(c) / filmic(vec3(LW)));
}

void main ()
{
  vec3 img_color = ( texture(texHDR,uv)).rgb;
  vec3 img_bloom =  vec3(( texture(texBLUM,uv)).r);
  vec3 img_ssao =  vec3( texture(texSSAO,uv).r);//use single color
  vec4 lumdata =   (texture(texLumKey,vec2(0.5,0.5))).rgba;//use single color
	

  float lum_key = lumdata.r;
  /*bloom + img with gamma correction*/
  color = vec4(img_ssao*gamma_v3((img_bloom +img_color)/((1.0+lum_key*0.5))) ,1.0);
 //color = vec4( img_ssao,1.0);
///color = vec4( vec3(lum_max),1.0);
// color = vec4( img_color,1.0);
 //color = vec4( img_bloom,1.0);
 //color = vec4(gamma_v3(img_bloom),1.0);
//color = vec4(gamma_v3(img_color),1.0);
  }