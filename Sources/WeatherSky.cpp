#include "WeatherSky.h"
void SWeatherSky::SetTime(float LT) {



    SolAng a = SolarAngleModel(LT,local_GMT_delta,local_day_of_year,local_long,local_att);
    float sun_x = sin(a.Elev)*cos(a.Ath);
    float sun_y = sin(a.Elev)*sin(a.Ath);
    float sun_z = cos(a.Elev);


    sun_light_direction = vec4(sun_x, sun_y , sun_z,1.0);

    sky_dome_prog->SetUniform("sky_sunpos",vec4(sun_x,sun_y,sun_z,0.0) );
    sky_dome_cubemap_prog->SetUniform("sky_sunpos",vec4(sun_x,sun_y,sun_z,0.0) );
    //Debug info
    //printf("sun: %f %f %f\n Elev %f, Ath %f\n", sun_x,sun_y,sun_z,toDeg(a.Elev),toDeg(a.Ath));

}

vec4 SWeatherSky::GetSunDirection()
{
    return sun_light_direction;
}

void SWeatherSky::SetSunPos(const vec4& sun) {
    sky_dome_prog->SetUniform("sky_sunpos",sun);
    sky_dome_cubemap_prog->SetUniform("sky_sunpos",sun);
}

void SWeatherSky::SetTurbidity(float t) {
    sky_turbidity = t;
    sky_dome_prog->SetUniform("sky_turbidity",t);
    sky_dome_cubemap_prog->SetUniform("sky_turbidity",t);
}

void SWeatherSky::SetSunSize(float s)
{
    sky_disksize = s;
    sky_dome_prog->SetUniform("sky_sunsize",sky_disksize);
    sky_dome_cubemap_prog->SetUniform("sky_sunsize",sky_disksize);
}
SolAng SWeatherSky::SolarAngleModel(float LT,float delta_GMT,float d, float longtitude, float attitude) {
    SolAng pos;
    if (LT < 0)
        LT = 0;
    while (LT > 24)
        LT = LT - 24;
    float LSTM = toRad(15*delta_GMT);
    float B = toRad((360.0f/365.0f)*(d-81.0f));
    float EoT = 9.87f*sin(2*B)-7.53f*cos(B)-1.5f*sin(B);
    float TC = 4*(longtitude-LSTM)+ EoT; /*time correction*/
    float LST = LT + TC/60.0f; /* Local solar time*/
    float HRA = toRad(15*(LST-12.0f)); /*Hour angle*/
    float declination = toRad(23.45f)*sin(toRad((360.0f/365.0f)*(d-81.0f)));
    //float cosElev = sin(attitude)*sin(declination) + cos(attitude)*cos(declination)*cos(HRA);
    //pos.Elev = acos(cosElev);
    pos.Elev = asin( sin(declination) *sin(attitude)+ cos(declination)*cos(attitude)*cos(HRA));
    //pos.Elev = toDeg(90)-pos.Elev;
    pos.Ath = acos((sin(declination)*cos(attitude)- cos(declination)*sin(attitude)*cos(HRA))/cos(pos.Elev));
    pos.Elev = toRad(90.0f)-pos.Elev;

    if (LST > 12.0f || HRA > 0.0f)
        pos.Ath = toRad(360.0f)-pos.Ath;
    
    //pos.Ath = acos((sin(declination)*cos(attitude) - cos(HRA)*cos(declination)*sin(attitude))/ cos(toDeg(90)-pos.Elev));
    return pos;

}
SWeatherSky::SWeatherSky() {

    sky_dome_model = new SObjModel("AssetBase/sky_dome.obj");
    sky_dome_prog = new SShader("Sky/PerezSky.vert","Sky/PerezSky.frag");
    sky_dome_cubemap_prog = new SShader("Sky/PerezSkyCubemap.vert","Sky/PerezSkyCubemap.frag","Sky/PerezSkyCubemap.geom");

    sky_dome_model->ConfigureProgram( *sky_dome_prog);
    sky_dome_model->ConfigureProgram( *sky_dome_cubemap_prog);
    sky_dome_model->SetModelMat(SMat4x4().Scale(1000.0f,1000.0f,1000.0f));

    sky_dome_prog->SetUniform("sky_turbidity",sky_turbidity);
    sky_dome_prog->SetUniform("sky_sunsize",sky_disksize);
    sky_dome_prog->SetUniform("sky_sunpower",sky_sunpower);
    sky_dome_prog->SetUniform("sky_power",sky_power);
    sky_dome_prog->SetUniform("sky_sunpos",vec4( 0.0f,1.0f,0.0f,1.0f ));

    sky_dome_cubemap_prog->SetUniform("sky_turbidity",sky_turbidity);
    sky_dome_cubemap_prog->SetUniform("sky_sunsize",sky_disksize);
    sky_dome_cubemap_prog->SetUniform("sky_sunpower",sky_sunpower);
    sky_dome_cubemap_prog->SetUniform("sky_power",sky_power);
    sky_dome_cubemap_prog->SetUniform("sky_sunpos",vec4( 0.0f,1.0f,0.0f,1.0f ));
}

SWeatherSky::~SWeatherSky()
{
    delete sky_dome_model;
    delete sky_dome_prog;
    delete sky_dome_cubemap_prog;
}
void SWeatherSky::Draw(RenderContext& r) {
    if (sky_dome_model->IsReady)
    {
        sky_dome_model->Render(r);
    }

}
