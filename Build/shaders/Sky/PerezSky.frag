/* Base Lighting Shader */
#version 330
layout(location = 0) out vec4 dstColor;
const float pi = 3.14159;
//from ShaderFastMathLib
//
// Trigonometric functions
//
const  float fsl_PI = 3.1415926535897932384626433f;
const  float fsl_HALF_PI = 0.5f * fsl_PI;

// 4th order polynomial approximation
// 4 VGRP, 16 ALU Full Rate
// 7 * 10^-5 radians precision
// Reference : Handbook of Mathematical Functions (chapter : Elementary Transcendental Functions), M. Abramowitz and I.A. Stegun, Ed.
float acosFast4(float inX)
{
        float x1 = abs(inX);
        float x2 = x1 * x1;
        float x3 = x2 * x1;
        float s;

        s = -0.2121144f * x1 + 1.5707288f;
        s = 0.0742610f * x2 + s;
        s = -0.0187293f * x3 + s;
        s = sqrt(1.0f - x1) * s;

        // acos function mirroring
        // check per platform if compiles to a selector - no branch neeeded
        return inX >= 0.0f ? s : fsl_PI - s;
}

/* BRDF Required vectors*/
in vec3 o_normal;

//
// Perez zenith func
//
vec3 perezZenith ( float t, float thetaSun )
{
    const vec4  cx1 = vec4 ( 0,       0.00209, -0.00375, 0.00165  );
    const vec4  cx2 = vec4 ( 0.00394, -0.03202, 0.06377, -0.02903 );
    const vec4  cx3 = vec4 ( 0.25886, 0.06052, -0.21196, 0.11693  );
    const vec4  cy1 = vec4 ( 0.0,     0.00317, -0.00610, 0.00275  );
    const vec4  cy2 = vec4 ( 0.00516, -0.04153, 0.08970, -0.04214 );
    const vec4  cy3 = vec4 ( 0.26688, 0.06670, -0.26756, 0.15346  );

    float   t2    = t*t;
    float   chi   = (4.0 / 9.0 - t / 120.0 ) * (fsl_PI - 2.0 * thetaSun );
    vec4    theta = vec4 ( 1, thetaSun, thetaSun*thetaSun, thetaSun*thetaSun*thetaSun );

    float   Y = (4.0453 * t - 4.9710) * tan ( chi ) - 0.2155 * t + 2.4192;
    float   x = t2 * dot ( cx1, theta ) + t * dot ( cx2, theta ) + dot ( cx3, theta );
    float   y = t2 * dot ( cy1, theta ) + t * dot ( cy2, theta ) + dot ( cy3, theta );

    return vec3 ( Y, x, y );
}

vec3  perezFunc ( float t, float cosTheta, float cosGamma )
{
    float  gamma      = acosFast4 ( cosGamma );
    float  cosGammaSq = cosGamma * cosGamma;
    float  aY =  0.17872 * t - 1.46303;
    float  bY = -0.35540 * t + 0.42749;
    float  cY = -0.02266 * t + 5.32505;
    float  dY =  0.12064 * t - 2.57705;
    float  eY = -0.06696 * t + 0.37027;
    float  ax = -0.01925 * t - 0.25922;
    float  bx = -0.06651 * t + 0.00081;
    float  cx = -0.00041 * t + 0.21247;
    float  dx = -0.06409 * t - 0.89887;
    float  ex = -0.00325 * t + 0.04517;
    float  ay = -0.01669 * t - 0.26078;
    float  by = -0.09495 * t + 0.00921;
    float  cy = -0.00792 * t + 0.21023;
    float  dy = -0.04405 * t - 1.65369;
    float  ey = -0.01092 * t + 0.05291;

    return vec3 ( (1.0 + aY * exp(bY/cosTheta)) * (1.0 + cY * exp(dY * gamma) + eY*cosGammaSq),
                  (1.0 + ax * exp(bx/cosTheta)) * (1.0 + cx * exp(dx * gamma) + ex*cosGammaSq),
                  (1.0 + ay * exp(by/cosTheta)) * (1.0 + cy * exp(dy * gamma) + ey*cosGammaSq) );
}

vec3  perezSky ( float t, float cosTheta, float cosGamma, float cosThetaSun )
{
    float thetaSun = acosFast4        ( cosThetaSun );
    vec3  zenith   = perezZenith ( t, thetaSun );
    vec3  clrYxy   = zenith * perezFunc ( t, cosTheta, cosGamma ) / perezFunc ( t, 1.0, cosThetaSun );

    clrYxy [0] *= smoothstep ( 0.0, 0.5, cosThetaSun );         // make sure when thetaSun > PI/2 we have black color

    clrYxy [0] *= smoothstep ( 0.0, 0.1, cosTheta );         // make sure when theta > PI/2 we have black color

    //clrYxy [0] *= cosThetaSun;
    return clrYxy;

}


vec3	convertColor (vec3 p_color)
{
    vec3  clrYxy = vec3 ( p_color );
                                            // now rescale Y component
    clrYxy [0] = 1.0 - exp ( -clrYxy [0] / 25.0 );

    float ratio = clrYxy [0] / clrYxy [2];  // Y / y = X + Y + Z
    vec3  XYZ;

    XYZ.x = clrYxy [1] * ratio;             // X = x * ratio
    XYZ.y = clrYxy [0];                     // Y = Y
    XYZ.z = ratio - XYZ.x - XYZ.y;          // Z = ratio - X - Y

    const vec3 rCoeffs = vec3 ( 3.240479, -1.53715, -0.49853  );
    const vec3 gCoeffs = vec3 ( -0.969256, 1.875991, 0.041556 );
    const vec3 bCoeffs = vec3 ( 0.055684, -0.204043, 1.057311 );

    return vec3 ( dot ( rCoeffs, XYZ ), dot ( gCoeffs, XYZ ), dot ( bCoeffs, XYZ ) );
}

uniform vec4 sky_sunpos  = vec4(0.0,1.0,0.0,1.0);
uniform float sky_sunpower  = 10.0;
uniform float sky_sunsize = 100.0;
uniform float sky_turbidity = 2.0;
uniform float sky_power = 2.0;
void main() 
{

    vec3 sunpos = sky_sunpos.xyz;
    const vec3 zenitpos = vec3(0.0,1.0,0.0);

    vec3 n_sunpos = normalize(sunpos);
    vec3 n_o_normal = normalize(o_normal);
    vec3 n_zenitpos = normalize(zenitpos);
    //upper hemiosphere
    float cosGamma = max(dot(n_sunpos,n_o_normal),0.0);
    float cosTheta = max(dot(n_o_normal,n_zenitpos),0.0);
    float cosSunTheta = max(dot(n_sunpos,n_zenitpos),0.0);
    vec3 p_c = vec3(perezSky (sky_turbidity,cosTheta+0.09, cosGamma,cosSunTheta));
    // sun disk specular
    float sun_spec = pow( max(0.0f , cosGamma ), sky_sunsize)*clamp(cosTheta,0, 0.1)*sky_sunpower;
    dstColor = vec4(sky_power*(vec3(sun_spec)+max(convertColor(p_c),0.0)),1.0);
}
