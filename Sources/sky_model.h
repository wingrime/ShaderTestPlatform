#pragma once


#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/wglew.h>

/* shader class*/
#include "r_sprog.h"

#include "mat_math.h"

#include "viewport.h"
struct SolAng {
    float Elev;
    float Ath;
};

class SProcedureSky {
public:
    SProcedureSky (Viewport *vp);
    void Draw();
    SolAng SolarAngleModel(float LT,float delta_GMT,float d, float longtitude, float attitude);
    void SetTime(float time);
    void SetTurbidity(float t);
    void SetSunPos(const SVec4& sun);
    void SetCamera(const SMat4x4& m);

    /*working shader */
    SShader * sky_prog;

    SMat4x4 model;

    Viewport *v;
    GLuint vbo;
    GLuint vao;
    GLuint ibo;


    const float local_att = toRad(10);
    const float local_long = toRad(10);
    const float local_GMT_delta = -5;
    const float local_day_of_year = 10;

};
void SProcedureSky::SetTime(float LT) {



    SolAng a = SolarAngleModel(LT,local_GMT_delta,local_day_of_year,local_long,local_att);
    //sky_prog->Use ();
    float sun_x = sin(a.Elev)*cos(a.Ath);
    float sun_y = sin(a.Elev)*sin(a.Ath);
    float sun_z = cos(a.Elev);
    sky_prog->SetUniform("sun_pos",SVec4(sun_x,sun_y,sun_z,0.0) );

    printf("sun: %f %f %f\n Elev %f, Ath %f\n", sun_x,sun_y,sun_z,toDeg(a.Elev),toDeg(a.Ath));
    

    sky_prog->SetUniform("viewport",SVec4(v->w,v->h,0,0));

}


void SProcedureSky::SetCamera(const SMat4x4& m) {


    sky_prog->SetUniform("view",m);

}

void SProcedureSky::SetSunPos(const SVec4& sun) {


    sky_prog->SetUniform("sun_pos",sun);

}

void SProcedureSky::SetTurbidity(float t) {
 //   sky_prog->Use ();
    sky_prog->SetUniform("turbidity",t);

    sky_prog->SetUniform("viewport",SVec4(v->w,v->h,0,0));
}
SolAng SProcedureSky::SolarAngleModel(float LT,float delta_GMT,float d, float longtitude, float attitude) {
    SolAng pos;
    if (LT < 0)
        LT = 0;
    while (LT > 24)
        LT = LT - 24;
    float LSTM = toRad(15*delta_GMT);
    float B = toRad((360/365)*(d-81));
    float EoT = 9.87*sin(2*B)-7.53*cos(B)-1.5*sin(B);
    float TC = 4*(longtitude-LSTM)+ EoT; /*time correction*/
    float LST = LT + TC/60; /* Local solar time*/
    float HRA = toRad(15*(LST-12)); /*Hour angle*/
    float declination = toRad(23.45)*sin(toRad((360/365)*(d-81)));
    //float cosElev = sin(attitude)*sin(declination) + cos(attitude)*cos(declination)*cos(HRA);
    //pos.Elev = acos(cosElev);
    pos.Elev = asin( sin(declination) *sin(attitude)+ cos(declination)*cos(attitude)*cos(HRA));
    //pos.Elev = toDeg(90)-pos.Elev;
    pos.Ath = acos((sin(declination)*cos(attitude)- cos(declination)*sin(attitude)*cos(HRA))/cos(pos.Elev));
    pos.Elev = toRad(90)-pos.Elev;

    if (LST > 12 || HRA > 0)
        pos.Ath = toRad(360)-pos.Ath;
    
    //pos.Ath = acos((sin(declination)*cos(attitude) - cos(HRA)*cos(declination)*sin(attitude))/ cos(toDeg(90)-pos.Elev));
    return pos;

}
SProcedureSky::SProcedureSky(Viewport *vp) {
    sky_prog = new  SShader("f_shader.v","f_shader.f");
  
    GLfloat vertices[] = { -1, -1, 0, //bottom left corner
                           -1,  1, 0, //top left corner
                            1,  1, 0, //top right corner
                            1, -1, 0}; // bottom right rocner

    GLubyte indices[] = {0,1,2, // first triangle (bottom left - top left - top right)
                         0,2,3}; // second triangle (bottom left - top right - bottom right)
    
    v = vp;
    glGenVertexArrays ( 1, &vao );
    glBindVertexArray ( vao );

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData ( GL_ARRAY_BUFFER,sizeof(GLfloat)*12, vertices, GL_STATIC_DRAW);




    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,  sizeof(GLubyte)*6, indices, GL_STATIC_DRAW);

   // sky_prog->Bind ();
 
    glEnableVertexAttribArray(0);

    sky_prog->SetAttrib( "initial_vertex_coord", 3, 0,0,GL_FLOAT);
    //n_tex -> Select();
 
    sky_prog->SetUniform("turbidity",(float)2.0);

    sky_prog->SetUniform("zenit_pos",SVec4(0,0,1,0) );

    sky_prog->SetUniform("sun_pos",SVec4( 0.1,0.3,1,0) );


    model = model.RotX((3.14159/180)*90);
 

    sky_prog->SetUniform("model",model);

    sky_prog->SetUniform("view",SMat4x4());


    sky_prog->SetUniform("viewport",SVec4(v->w,v->h,0,0));

// sky_prog->ReflectUniforms();


}
void SProcedureSky::Draw() {
    /*Quard*/
    /*If shader usable other wice we will get error*/
    if (sky_prog->IsReady)
    {
        glBindVertexArray ( vao );
        sky_prog-> Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, (GLvoid *)0);
        glBindVertexArray ( 0 );
    }


    

}
