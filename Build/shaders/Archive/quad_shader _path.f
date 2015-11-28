#version 330
uniform vec4 vp; /* viewport conf*/
uniform float time;
layout(location = 0) out vec4 color;
struct mat_t {
//vec3 color;
int idx;
};
struct plane_t {
	vec3 n;
    float d;
    mat_t mat;
};
struct sphere_t {
    vec3 sc;
    float sr;
    mat_t mat;
};
struct rr_t {
    vec3 n; /*normal*/
    float d; /*dist*/
     mat_t mat;
    
};    
/*lambert BRDF */
float lambert(vec3 n, vec3 l) {
        return max(0.0, dot(normalize(n),normalize(l)));
}
const float INF = 33333.0;
/*iq optimized sphere intersect sr - radius sc - position*/
float iSphere(sphere_t s, vec3 ro, vec3 rd) {
    vec3 oc = ro - s.sc;
    float b = dot(rd,oc);
    float c = dot(oc,oc ) - s.sr*s.sr;
    float t = b*b - c;
    if (t > 0.0)
        t = -b - sqrt(t);
    if (t < 0.0001)
       return INF;
    return t;
}
float iPlane(plane_t plane , vec3 ro, vec3 rd) {
    float s = (-(dot(plane.n,ro)+plane.d))/ (dot(plane.n,rd)+0.0001);
    if (s < 0.0001)
        return INF;
    
    return s; 
}
sphere_t s1;
sphere_t s2;
sphere_t s3;
sphere_t s4;
plane_t p[7];
	
vec2 uv;
vec2 mouse;
vec3 light;

float seed;
float random() {
    seed = fract(sin(seed)*265871.1723);
    
 	return (seed*2.0)-1.0;
}
    
    
rr_t rayIntersect(vec3 ro, vec3 rd) {
    vec3 n = vec3(0.0);
    float rp = INF;
    mat_t r_mat;
    float rr;
  
    
    rr = iSphere(s1,ro,rd);
    if (rp > rr ){rp = rr;  n = normalize( (ro+rd*rr) - s1.sc); r_mat = s1.mat; }
    
    rr = iSphere(s2,ro,rd);
    if (rp > rr ) {rp = rr;  n = normalize( (ro+rd*rr) - s2.sc);r_mat = s2.mat; }
    
    rr = iSphere(s3,ro,rd);
    if (rp > rr ) {rp = rr;  n = normalize( (ro+rd*rr) - s3.sc);r_mat = s3.mat; }

    rr = iSphere(s4,ro,rd);
    if (rp > rr ) {rp = rr;  n = normalize( (ro+rd*rr) - s4.sc);r_mat = s4.mat; }
    
   rr = iPlane(p[0],ro,rd);
   if (rp > rr ) {rp = rr;  n = p[0].n;r_mat = p[0].mat; }
   
   rr = iPlane(p[1],ro,rd);
   if (rp > rr ) {rp = rr;  n = p[1].n;r_mat = p[1].mat; }
    
   rr = iPlane(p[2],ro,rd);
   if (rp > rr ) {rp = rr;  n = p[2].n;r_mat = p[2].mat; }
   
   rr = iPlane(p[3],ro,rd);
   if (rp > rr ) {rp = rr;  n = p[3].n;r_mat = p[3].mat; }
   
   rr = iPlane(p[4],ro,rd);
   if (rp > rr ) {rp = rr;  n = p[4].n;r_mat = p[4].mat; }
   
   rr = iPlane(p[5],ro,rd);
   if (rp > rr ) {rp = rr;  n = p[5].n;r_mat = p[5].mat; }
   return rr_t( n , rp, r_mat);
    
}

float grandom() {
  return cos(6.28*random()*sqrt(-2.0*log(random())));   
}

vec3 getBRDFRay(vec3 n,vec3 rd) {
vec3 rvec = normalize(vec3(random(), random(), random()));
	
    //rvec *= sign(dot(rvec,n));
    //return rvec;
    return reflect(mix(rd,rvec,0.1),n);
}
float isLightVisible(vec3 point) {
vec3 from_light_dir = normalize( point-light);
vec3 l_occ = light+from_light_dir* rayIntersect( light ,from_light_dir ).d;
return length(point-l_occ);
#define D_MAT  0
#define G_MAT  2
#define E_MAT  1
}
vec3 rayTrace(vec3 ro_in, vec3 rd_in) {
	
    vec3 color_res = vec3(0.0);
    vec3 dir_color = vec3(0.0);
    vec3 ro = ro_in;
    vec3 rd = rd_in;
    float path = 0.0;
    vec3 clr = vec3(1.0);
    const int RAY_D = 4;
    float intense_gather = 0.0;
    vec3 color_gather = vec3(0.0);
    for (int i = 0; i < RAY_D ; i++) {
    
    	rr_t rc = rayIntersect(ro,rd);
       	dir_color = vec3(0.0);
        
        path = path + abs(rc.d);
        ro = ro+rd*rc.d;
        rd = getBRDFRay(rc.n,rd);
        
        vec3 light_dir = normalize(light-(ro) );
        //if (rc.mat.idx == 2)
        //	dir_color = vec3(0.0,1.0,0.0)* lambert(-rc.n,light_dir);
        //else
        float shadow_m = 1.0;
        //if (isLightVisible(ro)>0.01){
        // shadow_m =0.6;
        //}
        float ray_decay = 1.0/(0.5*float(i)+1.0);
        vec3 col;
        float intense =0.0;

                if (rc.mat.idx == G_MAT)
                    col = vec3(0.0,1.0,0.0)* lambert(rc.n,light_dir)*shadow_m;
                else if (rc.mat.idx == D_MAT)
                    col = vec3(0.5)* lambert(rc.n,light_dir)*shadow_m;
                else{
                    //dir_color = vec3(1.0)* lambert(rc.n,light_dir)*shadow_m;
                    // intense = ray_decay;
                    col = vec3(0.5)* lambert(rc.n,light_dir)*shadow_m;
                    //ro = ro_in;
                    //rd = rd_in;
                    //path = 0.0;
                    }


        /*exponential decay*/
        color_res = color_res + dir_color;//*exp(-0.11*path);
        intense_gather += intense;
        color_gather = color_gather + col*ray_decay;

    }
        
        
    return color_gather/RAY_D;
}
float tonemap(float hdr) {
    float h = hdr*1.0 ;//exposure
    float gc = h/(1.0+h);
    return pow(gc, 1.0/2.2);
}
vec3 color_c(vec3 h) {
return vec3(tonemap(h.r), tonemap(h.g), tonemap(h.b));
}

void main(void)
{

    uv = ((gl_FragCoord.xy / vp.xy)*2.0 - 1.0);
    
    //mouse = (iMouse.xy/ iResolution.xy )*2.0 - 1.0;
    seed = uv.x*110.0880+uv.y*0.2+mouse.x*1.44+ mouse.y*0.777+(12.1+time);
    //seed = random()+1.0/iGlobalTime;
    //light = (vec3(mouse.x+0.2*sin(iGlobalTime),mouse.y+0.1*cos(iGlobalTime),0.8*sin(iGlobalTime)));
    //light = (vec3(mouse.x,mouse.y,0.0));
    //light = vec3(0.0,0.0,0.0);
    light = (vec3(0.2*sin(time),0.1*cos(time),0.8*sin(time)));
    /*obj*/
    mat_t def_mat = mat_t ( 0 ) ;
    mat_t emm_mat = mat_t ( 1 ) ;
    mat_t g_mat = mat_t ( 2 ) ;
    
    /*init*/
    s1 = sphere_t(vec3(0.5, 0.0,0.0), 0.25,def_mat);
    s2 = sphere_t(vec3(-0.5, 0.0,0.0), 0.25,def_mat);

    s3 = sphere_t((vec3(-0.2*sin(2.0*time),-0.1*cos(2.0*time),-0.8*sin(2.0*time))), 0.25,g_mat);
    s4 = sphere_t(vec3(0.0, -0.75,0.0), 0.25,def_mat);
    
    p[0] = plane_t(vec3(0.0,1.0,0.0),1.0,def_mat);
    p[1] = plane_t(vec3(1.0,0.0,0.0),1.0,g_mat);
    p[2] = plane_t(vec3(-1.0,0.0,0.0),1.0,g_mat);
    p[3] = plane_t(vec3(0.0,0.0,-1.0),1.0,g_mat);
    p[4] = plane_t(vec3(0.0,-1.0,0.0),1.0,emm_mat);
    p[5] = plane_t(vec3(0.0,0.0,1.0),-2.0,g_mat);
    
    /*pinhole camera pos*/
    vec3 ph_pos = vec3(0.0, 0.0, -3.0);
    //vec3 ph_pos = vec3(uv.x , uv.y , -2.0);
    const float eps = 0.001;
    /*proj plane*/
    vec3 prj_pos = vec3(uv.x , uv.y , -1.0);
    /*ray direction*/
    vec3 rd = normalize(prj_pos - ph_pos);
    vec3 ro = ph_pos;
    vec3 res_color = vec3(0.0);
    const int S = 3;
    
    for (int i = 0; i< S ; i++)
    	res_color += rayTrace(ro, rd);
    res_color /= float(S);


     
    color = vec4(color_c(res_color),1.0);
}
