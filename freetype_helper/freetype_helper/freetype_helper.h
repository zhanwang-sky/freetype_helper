//
//  freetype_helper.h
//  freetype_helper
//
//  Created by zhanwang-sky on 2024/8/2.
//

#ifndef freetype_helper_h
#define freetype_helper_h

#include <ft2build.h>
#include FT_FREETYPE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct freetype_context {
  FT_Library library;
  FT_Face face;
  int ascender;
  int descender;
} freetype_context_t;

int freetype_load_font(freetype_context_t* ctx, const char* path);

int freetype_set_pxsize(freetype_context_t* ctx, int width, int height);

int freetype_render_ascii(freetype_context_t* ctx,
                          unsigned char* canvas, int width, int height,
                          const char* ascii_text, int len);

int freetype_render_unicode(freetype_context_t* ctx,
                            unsigned char* canvas, int width, int height,
                            const unsigned long* unicode_text, int len);

void freetype_free(freetype_context_t* ctx);

#ifdef __cplusplus
}
#endif

#endif /* freetype_helper_h */
