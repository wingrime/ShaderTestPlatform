#pragma once 
/*
TODOs:
- use factory to join same fonts
- print mem should be moved to some proper place
- count lines writed and retrun result
*/
#include <string>
#include <map>
/*error classes */
#include "e_base.h"

/*file buffer class*/
#include "c_filebuffer.h"

#include "r_sprog.h"

/*free type 2*/
#include "ft2build.h"
#include FT_FREETYPE_H
#define DEF_fontname "Anonymous Pro.ttf"
#define DEF_fontsize 16



class UIFont {
	public: 
		UIFont(FT_Library &lib,const std::string& fnt_name, unsigned const int sz) ;

		int RenderText(const std::string& text, float x_uv, float y_uv, float vp_sx, float vp_sy);

  private:
	FT_Library ft;

	FT_Face face;

	GLuint tex;

	SProg * ui_prog;
	GLuint vbo;
	GLuint vao;

  struct CharData {
    unsigned int tex_id;
    int advance_x;
    int advance_y;
    int bitmap_left;
    int bitmap_top;
    int bitmap_width;
    int bitmap_rows;

  }; 
  typedef std::unordered_map<char, CharData> TexMap;
  typedef std::pair<char, CharData> TexPair;



  TexMap texmap;


  unsigned int BitMapTexture(int w, int r, unsigned char* buffer);
};


UIFont::UIFont(FT_Library &lib,const std::string& fnt_name,unsigned const int sz) {


	ft = lib;

	/*TODO use filebuffer*/
	if(FT_New_Face(UIFont::ft, fnt_name.c_str(), 0, &face))
		throw FontLoadError("Could not open font\n");

 //FT_Select_Charmap( face, ft_encoding_unicode  );

	FT_Set_Pixel_Sizes(face, sz, 0);

	/*init ui shaders*/

	ui_prog = new  SProg("ui_shader.v","ui_shader.f");



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
  ui_prog->SetUniform("color",SVec4(1,1,1,1));
  ui_prog->SetAttrib( "coord", 2, sizeof(float)*2, (void *) (0),GL_FLOAT);

 glBindVertexArray ( 0 );

}

void print_mem(unsigned char const *vp, size_t n)
{
    unsigned char const *p = vp;
    for (size_t i=0; i<n; i++)
    	if (p[i])
        	printf("%x\n", p[i]);
    putchar('\n');
};
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


 
  float sx = 2.0 / vp_sx;
  float sy = 2.0 / vp_sy;

  float x = sx + (x_uv-0.5)*2 ;
  float y = sy - (y_uv-0.5)*2;

  float start_x = x;
  float start_y = y;

  glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBindVertexArray ( vao);
  glBindBuffer ( GL_ARRAY_BUFFER, vbo );
  ui_prog->Bind();
	int line_gap = (face->size->metrics.height - (face->size->metrics.ascender + face->size->metrics.descender))*3.0;	

	
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

   	    x =start_x;
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

    ui_prog->SetUniform("quad_size",SVec4(x2,y2,w,h));
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


class UIFontFactory {
public: 
	UIFontFactory();

	UIFont *Construct(const std::string& fnt_name,unsigned const int sz);
  UIFont *Construct(unsigned const int sz);
	FT_Library ft;
};
UIFontFactory::UIFontFactory() {
		if(FT_Init_FreeType(&ft)) 
	
		throw FontLoadError("Could not init freetype library\n");
}

UIFont *UIFontFactory::Construct(const std::string& fnt_name, unsigned const int sz) {
	return new UIFont(ft,fnt_name,sz);
}

UIFont *UIFontFactory::Construct( unsigned const int sz) {
  return new UIFont(ft,DEF_fontname,sz);
}
