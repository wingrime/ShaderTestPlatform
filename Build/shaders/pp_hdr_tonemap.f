#version 330
uniform vec4 vp; /* viewport conf*/
layout(location = 0) out vec4 color;
uniform sampler2D texSRC1;
uniform sampler2D texSRC2;
uniform sampler2D texSRC3;
#define texHDR texSRC2
#define texBLUM texSRC1
#define texSSAO texSRC3
uniform float aoStrength = 1.0;
uniform float A = 0.22;
uniform float B = 0.30;
uniform float C = 0.10;
uniform float D = 0.20;
uniform float E = 0.01;
uniform float F = 0.30;
uniform float LW = 11.2;
float filmic(float x) {
  return ((x*(A*x+C*B)+D*E)/ (x*(A*x+B)+D*F))-E/F;
}

/* gamma correction */
float sRGB(float x) {
  if ( x <= 0.00031308)
    return 12.92*x;
  else
    return 1.055*pow(x,1/2.4) - 0.055;
}
float gamma(float x) {
if (x < 0)
  return 0;

float f = filmic(x) / filmic(LW);
return sRGB(f);

  //float rein = x/(x+1);
  //  return pow(rein,1/2.2);
  //reinhat operator

}

vec3 gamma_v3( vec3 i) {
  return vec3(gamma(i.x),gamma(i.y),gamma(i.z));
}

void main ()
{
  vec2 c = gl_FragCoord.xy/vp.xy;
  vec3 img_color = ( texture(texHDR,c)).rgb;
  vec3 img_bloom =  ( texture(texBLUM,c)).rgb;
  vec3 img_ssao =  ( texture(texSSAO,c)).rgb;
	
  /*bloom + img with gamma correction*/
  color = vec4(img_ssao*gamma_v3((img_bloom +img_color)  ),1.0);
 //color = vec4( img_ssao,1.0);
// color = vec4( img_color,1.0);
 //color = vec4( img_bloom,1.0);
 //color = vec4(gamma_v3(img_bloom),1.0);
//color = vec4(gamma_v3(img_color),1.0);
  }