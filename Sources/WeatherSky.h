#pragma once
/*
 * Sky Model
*/
#include "r_sprog.h"
#include "r_shader.h"
#include "mat_math.h"
#include "ObjModel.h"
#include "r_projmat.h"

/*System includes*/
#include "math.h"
#include <GL/glew.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "RenderContext.h"

struct SolAng {
    float Elev;
    float Ath;
};

class SWeatherSky {
public:
    SWeatherSky ();
    ~SWeatherSky ();
    SWeatherSky (const SWeatherSky&) = delete;
    void Draw(RenderContext &r);
    SolAng SolarAngleModel(float LT,float delta_GMT,float d, float longtitude, float attitude);
    void SetTime(float time);
    void SetTurbidity(float t);
    void SetSunSize(float t);
    void SetSunPos(const vec4& sun);



    SShader * GetSkyShader() {return sky_dome_prog;}
    SShader * GetSkyCubemapShader() {return sky_dome_cubemap_prog;}
    SObjModel * GetSkyModel() {return sky_dome_model;}
    const SMat4x4  GetSkyProjectionMatrix() {return matrix_projection;}

    vec4 GetSunDirection();

private:

    SMat4x4 model;
    SObjModel *sky_dome_model;
    SShader  *sky_dome_prog;
    SShader  *sky_dome_cubemap_prog;


    SMat4x4 matrix_projection = SInfinityFarProjectionMatrix(100.0f,1.0f,toRad(26.0));

    vec4 sun_light_direction = vec4(0.5,0.5,0.0,1.0);


    /*Sun location model*/
    float local_att = toRad(56.0);
    float local_long = toRad(60.0);
    float local_GMT_delta = -5;
    float local_day_of_year = 150;
    /*Sky model*/
    float sky_turbidity = 2.0;
    float sky_disksize = 100.0;
    float sky_sunpower  = 10.0;
    float sky_power = 2.0;

};
