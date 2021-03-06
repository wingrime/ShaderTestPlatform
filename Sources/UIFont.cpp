#include "UIFont.h"
/*file buffer class*/
#include "FileBuffer.h"
#include "mat_math.h"
#include "r_shader.h"
#include "Log.h"
#include "ErrorCodes.h"

UIFont::UIFont(FT_Library &lib,const std::string& fnt_name,unsigned const int sz) {

	ft = lib;

	/*TODO use filebuffer*/
    if(FT_New_Face(UIFont::ft, fnt_name.c_str(), 0, &face))
    {
        LOGE("Font not loaded!");
        return;
    }

    FT_Set_Pixel_Sizes(face, sz, 0);
    float scale = 1000.0f;
    FT_Matrix matrix = { ( int)((1.0f/scale) * 0x10000L), (int)((0.0f)* 0x10000L),(int)((0.0f) *0x10000L),( int)((1.0f) * 0x10000L) };
    FT_Set_Char_Size( face, (int)(sz  *64), 0, 150 * (int)scale, 150 );
    FT_Set_Transform( face, &matrix, NULL );
	/*init ui shaders*/

    //FT_Set_Char_Size( face, (int)(sz *64.0), 0, 150 , 150 );
    //FT_Set_Pixel_Sizes(face, 0, sz);


    ui_prog = new  SProg("UI/UIText.vert","UI/UIText.frag");

    glGenVertexArrays ( 1, &vao );
    glBindVertexArray ( vao );
	

	glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*8, 0, GL_STATIC_DRAW); /* Create Empty VBO*/

      GLfloat box[4][2] = {
        { 0, 0},
        { 1, 0},
        { 0, 1},
        { 1, 1},
    };
    glBufferSubData(GL_ARRAY_BUFFER, 0,sizeof(box), box); /*initial Fill*/
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
 
    FT_Select_Charmap(face,FT_ENCODING_ADOBE_STANDARD);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    ui_prog->Bind();
    ui_prog->SetUniform("tex",0);
    ui_prog->SetUniform("color",vec4(1,1,1,1));
    ui_prog->SetAttrib( "coord", 2, sizeof(float)*2, 0,GL_FLOAT);

    glBindVertexArray ( 0 );

    IsReady = true;

}

unsigned int UIFont::BitMapTexture(int w, int r, unsigned char* buffer) {

    unsigned int texID;
    /* Create new texture */
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        w,
        r,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        buffer
        );
    glBindTexture(GL_TEXTURE_2D, 0);
    return texID;
}
int UIFont::RenderText(const std::string& text, float x_uv, float y_uv, float vp_sx, float vp_sy) {

    if (!IsReady)
        return -1L;
    float sx = 1.0f / vp_sx;
    float sy = 1.0f / vp_sy;

    float x = sx + (x_uv-0.5f)*2.0f ;
    float y = sy - (y_uv-0.5f)*2.0f;

    float start_x = x;
    //float start_y = y;
    /*should be moved to state*/
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindVertexArray ( vao);
    glBindBuffer ( GL_ARRAY_BUFFER, vbo );
    ui_prog->Bind();
    /*problematic*/
    //int line_gap = (face->size->metrics.height - (face->size->metrics.ascender + face->size->metrics.descender));
    int line_gap = (face->size->metrics.height );//
    const char *p;

    FT_GlyphSlot g;
    for(p = text.c_str(); *p; p++) {

        glActiveTexture(GL_TEXTURE0);
        CharData t;
        if (texmap.find(*p) == texmap.end() )
        {
            if(FT_Load_Char(face, *p, FT_LOAD_RENDER))
                continue;

            g = face->glyph;
            if (*p == '\n') {
                x = start_x;
                /*FIX ME: looks like not adequate*/
                y -=((line_gap )>>6) * sy; //(g->metrics.horiBearingX);// >> 6) * sy;
                continue;
            }
            unsigned int texID = BitMapTexture(g->bitmap.width,g->bitmap.rows,g->bitmap.buffer);
            /* Create new texture */
            t.tex_id = texID;
            t.bitmap_left = g->bitmap_left;
            t.bitmap_top = g->bitmap_top;
            t.bitmap_width = g->bitmap.width;
            t.bitmap_rows = g->bitmap.rows;
            t.advance_x = g->advance.x;
            t.advance_y = g->advance.y;
            texmap.insert(TexPair(*p,t));
    } else {
            t = texmap[(*p)];
    }
    glBindTexture(GL_TEXTURE_2D, t.tex_id);
    /*Build quad*/
    float x2 = x + t.bitmap_left * sx;
    float y2 = y + t.bitmap_top * sy;
    float w = t.bitmap_width * sx;
    float h = t.bitmap_rows * sy;

    ui_prog->SetUniform("quad_size",vec4(x2,y2,w,h));
   // GLfloat box[4][4] = {
   //     {x2,     y2    , 0, 0},
   //     {x2 + w, y2    , 1, 0},
   //     {x2,     y2 - h, 0, 1},
   //     {x2 + w, y2 - h, 1, 1},
   // };

   // glBufferSubData(GL_ARRAY_BUFFER, 0,sizeof(box), box); /*Refresh data*/
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
   
    x += (t.advance_x >> 6) * sx;
    y += (t.advance_y >> 6) * sy;

  }

  glBindVertexArray ( 0 );
  return 0;
}
UIFontFactory::UIFontFactory() {
    FT_Init_FreeType(&ft);
}
UIFont *UIFontFactory::Construct(const std::string& fnt_name, unsigned const int sz) {
	return new UIFont(ft,fnt_name,sz);
}

UIFont *UIFontFactory::Construct( unsigned const int sz) {
    return new UIFont(ft,DEF_fontname,sz);
}

int UIFontFactory::Release(UIFont *fnt)
{
    delete fnt;
    return ESUCCESS;

}
