#version 330
uniform vec4 vp; /* viewport conf*/
layout(location = 0) out vec4 color;
uniform sampler2D texSRC1;
uniform sampler2D texSRC2; /*old result*/
uniform float eyeAdoptSpeed = 0.008;
void main ()
{ 
	float old_r = (texture(texSRC2,vec2(0.5,0.5))).x;
	float result_max = 0.0;
	float result_avg = 0.0;
  const int samples = 2; /*4 sample */
  for (int cx = 0; cx < samples; cx++) {
  	  for (int cy = 0; cy < samples; cy++) {
  	  	float s = (texture(texSRC1,vec2((float(cx))/float(samples),(float(cy))/float(samples)))).x;
  	  	result_max = max(result_max,s);
  	}
  }

  color = vec4(old_r + (result_max - old_r)*eyeAdoptSpeed,1.0,1.0,1.0);
}