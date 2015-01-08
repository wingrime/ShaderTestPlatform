#version 330
uniform vec4 vp; /* viewport conf*/
layout(location = 0) out vec4 color;
uniform sampler2D texSRC1;
uniform sampler2D texSRC2; /*old result*/
void main ()
{ 
	vec4 old_r = (texture(texSRC2,vec2(0.5,0.5)));
	float result_max = 0.0;
	float result_avg = 0.0;
  const int samples = 8;
  for (int cx = 0; cx < samples; cx++) {
  	  for (int cy = 0; cy < samples; cy++) {
  	  	vec2 s = (texture(texSRC1,vec2((float(cx)+0.1)/float(samples+1),(float(cy)+0.1)/float(samples+1) )  )).rg;
  	  	result_max = max(result_max,s.x);
  	  	result_avg = (s.y+result_avg)/2.0 ;
  	}
  }

  color = vec4((result_max - (result_max-old_r.x )*0.20),result_avg, result_max,1.0);
}