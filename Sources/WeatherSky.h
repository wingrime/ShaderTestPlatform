#pragma once
/*
 * Sky Model
*/
#include "Shader.h"
#include "r_shader.h"
#include "mat_math.h"
#include "ObjModel.h"
#include "ProjectionMatrix.h"

/*System includes*/
#include "math.h"
#include "OGL.h"

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


    SMat4x4 matrix_projection = SInfinityFarProjectionMatrix(100.0f,1.0f,toRad(26.0f));

    vec4 sun_light_direction = vec4(0.5f,0.5f,0.0f,1.0f);


    /*Sun location model*/
    float local_att = toRad(56.0f);
    float local_long = toRad(60.0f);
    float local_GMT_delta = -5.0f;
    float local_day_of_year = 150.0f;
    /*Sky model*/
    float sky_turbidity = 2.0f;
    float sky_disksize = 100.0f;
    float sky_sunpower  = 10.0f;
    float sky_power = 2.0f;

};
