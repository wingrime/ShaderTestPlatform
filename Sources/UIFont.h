#pragma once 
/*
TODOs:
- use factory to join same fonts
- print mem should be moved to some proper place
- count lines writed and retrun result
*/
#include <string>
#include <unordered_map>



/*free type 2*/
#include "ft2build.h"
#include FT_FREETYPE_H
#define DEF_fontname "Anonymous Pro.ttf"
#define DEF_fontsize 20

class SProg;

class UIFont {
	public: 
		UIFont(FT_Library &lib,const std::string& fnt_name, unsigned const int sz) ;

		int RenderText(const std::string& text, float x_uv, float y_uv, float vp_sx, float vp_sy);

        bool IsReady = false;
  private:
	FT_Library ft;

	FT_Face face;

    unsigned int tex;

	SProg * ui_prog;
    unsigned int vbo;
    unsigned int vao;

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

class UIFontFactory {
public: 
	UIFontFactory();

	UIFont *Construct(const std::string& fnt_name,unsigned const int sz);
  UIFont *Construct(unsigned const int sz);
  int Release( UIFont * fnt);
	FT_Library ft;
};
