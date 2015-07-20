#version 330
uniform mat4 matrixProjection;
uniform mat4 model;
uniform mat4 view;

uniform mat4 MVP;
uniform mat4 MV;
uniform vec4 main_light_dir = vec4(0.0,1.0,0.0,1.0);

uniform mat4 matrixShadowMapProjection;
uniform mat4 matrixShadowMapView;

/*input vertex format*/
in vec3 normal;
in vec3 position;
in vec2 UV;

/*Camera Space position*/
out vec3 PositionCS;
/*Model Space texture coordinates*/
out vec2 UvMS;

out vec3 NormalMS;
out vec3 PositionMS;
out vec3 NormalCS;
out mat4 invMatrixProjection;
flat out vec3 LightCS;


void main(void)
{
    /*perform lerp for vertex position*/
    PositionMS =  position;
    /*perform lerp for vertex normal*/
    NormalMS = normal;
    /*perform lerp for vertex normal*/
    /*fixup UV coordinates ?*/
    UvMS = vec2(1.0,-1.0)*UV;
    invMatrixProjection = inverse(matrixProjection);
    NormalCS = ((MV*vec4(normal,0.0)).xyz);
    PositionCS = (MV*vec4(position,1.0)).xyz;
    LightCS  =( MV*main_light_dir).xyz;

    gl_Position  = MVP*vec4(position,1.0);
}
