#version 330
uniform vec4 vp; /* viewport conf*/
uniform float time;
layout(location = 0) out vec4 color;


/*bob jankins one at time hash*/
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

vec3 l = normalize( vec3(1.0,1.0 ,-2.0) );

float sdFloat(vec3 p, float s) {
	return length(p)-s;
}

float sdBox(vec3 p, vec3 b) {
        return length(max (abs(p) -b ,0.0) );
}

float lambert(vec3 n , vec3 l) {
	return max(dot(n, l) , 0.0);
}
vec2 rot(vec2 r,float ang) {
    return vec2(r.x* cos(ang) - r.y * sin(ang),  r.x*sin(ang) + r.y* cos(ang) ) ;
}
float obj(vec3 rp) {
vec2 rotx =   rot(vec2(rp.x,rp.z) ,3.0*sin(time));

vec3 trn = vec3(1.0,1.0,1.0);

vec3 nrp1 = vec3(rotx.x ,rp.y , rotx.y );

vec3 nrp2 = nrp1+ vec3(0.0,0.8,0.0);


float nrp3 = sdBox(nrp2,vec3(5.5,.5,5.5))-0.1*abs(sin(nrp2.x*4)*sin(nrp2.z*4)*sin(nrp2.y*4)); //max(-sdFloat(nrp2,0.7), sdBox(nrp2, vec3(0.5,0.5 ,0.5 )  ) );


return  nrp3;
}

vec3 est_normal(vec3 rp ) {

const float diff = 0.0001;

vec3 x_p = vec3(diff ,0.0,0.0);
vec3 x_m = vec3(-diff ,0.0,0.0);

vec3 y_p = vec3(0.0,diff ,0.0);
vec3 y_m = vec3(0.0,-diff ,0.0);

vec3 z_p = vec3(0.0,0.0 ,diff);
vec3 z_m = vec3(0.0,0.0,-diff);


float x_diff = obj(rp+x_p) - obj(rp+x_m);
float y_diff = obj(rp+y_p) - obj(rp+y_m);
float z_diff = obj(rp+z_p) - obj(rp+z_m);

return normalize(vec3(x_diff,y_diff,z_diff));

}


vec3 raymatch_iter2(vec3 ro , vec3 rd ) {

        vec3 rp = ro;
        float s = 0.0;
        const int mstep = 100;
        const float eps = 0.0001;
        for (int i = 0; i < mstep ; i++) {

                s = obj(rp);

                rp = rp + rd*s;
        }

        vec3 n = est_normal(rp);
        vec3 ret = vec3(0.0);

        if (abs(s) < eps) {
            ret = vec3(1.0,1.0,1.0)*lambert(n, l);
         }
         else
            ret = vec3(0.0,0.0,1.0);

        return ret;

}

vec3 raymatch(vec3 ro , vec3 rd ) {

        vec3 rp = ro;
        float s = 0.0;
        const int mstep = 80;
        const float eps = 0.1;
	for (int i = 0; i < mstep ; i++) {

	
                s = obj(rp);

                rp = rp + rd*s;
               // if (abs(s) < eps ) break;
	
	}

        //vec3 n = normalize(vec3 (rp));

        vec3 n = est_normal(rp);
        vec3 ret = vec3(0.0);

        if (abs(s) < eps) {
            ret = vec3(0.9)*lambert(n, l)+vec3(0.1);
         }
         else
            ret = vec3(0.0,0.0,1.0);

        return ret;

}


void main ()
{
        vec2 uv = ((gl_FragCoord.xy/vp.xy)*2.0) - 1.0;
        seed = hash(1u+hash(  hash(floatBitsToUint(uv.x))+hash(hash(floatBitsToUint(uv.y))+hash(floatBitsToUint(time)))  ));


        /* projective*/
        vec3 ro = vec3(0.0 , 0.0 , -5.0 );
        /*ortogonal*/
        //vec3 ro = vec3(uv.x , uv.y , -2.0 );

        vec3 proj_plane = vec3 (uv.x , uv.y , -1.0 );

	vec3 rd = normalize( proj_plane - ro);


	vec3 res = raymatch(ro,rd);

	color = vec4(res,1.0);
}
