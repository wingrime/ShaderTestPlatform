/*perlite check*/
#version 330
uniform vec4 vp; /* viewport conf*/
uniform float time;
layout(location = 0) out vec4 color;
vec2 uv;


uint hash (uint x) {
x+= x<<10u;
x^= x>>6u;
x+= x<<3u;
x^= x>>11u;
x+= x<<15u;

return x;
}

float floatConstruct(uint m) {
    const uint ieeeMantissa = 0x007fffffu; // mantissa bitmask
    const uint ieeeOne = 0x3f800000u; // 1.0 in uint

m &= ieeeMantissa;
m |= ieeeOne;

float f = uintBitsToFloat(m);
return f - 1.0;
}
uint seed;
float rnd() {
seed = hash(seed);
return floatConstruct(seed);

}

float sd(vec2 uv) {

    return floatConstruct( hash(1u+hash(  hash(floatBitsToUint(uv.x))+hash(hash(floatBitsToUint(uv.y))+hash(floatBitsToUint(time)))  )));
}
vec3 per_noise(vec2 uv) {
    float o = 0.0;
    const float iters = 53;
    for (int i=0; i < iters;  i++) {
     o += sd(round(.964+uv*(1+float(i)))/(1+float(i)));
  }

    //o = sd(0.1+round(uv*5.0)/5.0);

    return vec3(o/float(iters));
}

void main(void)
{

    vec3 res_color = vec3(0.0f);

    uv = ((gl_FragCoord.xy / vp.xy)*2.0 - 1.0);
    seed = hash(1u+hash(  hash(floatBitsToUint(uv.x))+hash(hash(floatBitsToUint(uv.y))+hash(floatBitsToUint(time)))  ));


    res_color = per_noise(uv);
     
    color = vec4(res_color,1.0);
}
