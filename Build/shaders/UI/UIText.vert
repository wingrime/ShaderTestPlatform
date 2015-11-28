#version 330

in vec4 coord;
out  vec2 texcoord;
uniform vec4 quad_size;
void main(void) {

  //gl_Position = vec4(coord.xy, 0, 1);

  if (gl_VertexID == 0)
  	gl_Position = vec4(quad_size.xy, 0, 1);
  else if (gl_VertexID == 1)
  	gl_Position = vec4(quad_size.x + quad_size.z , quad_size.y , 0, 1);
  else if (gl_VertexID== 2)
  	gl_Position = vec4(quad_size.x , quad_size.y - quad_size.w , 0, 1);
  else if (gl_VertexID == 3)
  	gl_Position = vec4(quad_size.x + quad_size.z , quad_size.y - quad_size.w , 0, 1);
  //texcoord = coord.zw;
  texcoord = coord.xy;
}