#version 330
uniform vec4 vp; /* viewport conf*/
layout(location = 0) out vec4 color;
uniform sampler2D texSRC1;

void main ()
{
  vec2 texcord = gl_FragCoord.xy/vp.xy;
  vec4 c = texture(texSRC1,texcord);
  	/*ITU-R */
  //float lum = 0.2126*c.r + 0.7152*c.g + 0.0722*c.b;
	/*CCIR601*/
	float lum = 0.299*c.r + 0.587*c.g + 0.114*c.b;
  /*
  ITU-R
  	Y = 0.2126 R + 0.7152 G + 0.0722 B
  CCIR601
	Y = 0.299 R + 0.587 G + 0.114 B
  HSP Color model
	sqrt(0.299 * R^2 + 0.587 * G^2 + 0.114 * B^2)
  */

  color = vec4( lum, 0.0,0.0,1.0);
}