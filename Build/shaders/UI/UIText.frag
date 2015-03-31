#version 330
in vec2 texcoord;
uniform sampler2D tex;
uniform vec4 color;
out vec4 o_color;
void main(void) {
  o_color =    vec4(color.r,color.g,color.b,1.0)*texture2D(tex, texcoord).r  + vec4(0.0,0.0,0.0,1.0)*texture2D(tex, (texcoord-vec2(0.07,0.07))).r;
  o_color.a = clamp(o_color.a * 1.5, 0.0, 1.0);
}
