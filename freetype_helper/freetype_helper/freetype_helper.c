//
//  freetype_helper.c
//  freetype_helper
//
//  Created by zhanwang-sky on 2024/3/21.
//

#include "freetype_helper.h"

static void my_draw_bitmap(unsigned char* canvas, unsigned width, unsigned height,
                           int x_off, int y_off, FT_Bitmap* bitmap) {
  unsigned pen_x_off = x_off < 0 ? 0 : x_off;
  unsigned pen_y_off = y_off < 0 ? 0 : y_off;
  unsigned glyph_x_off = x_off < 0 ? -x_off : 0;
  unsigned glyph_y_off = y_off < 0 ? -y_off : 0;

  for (unsigned pen_y = pen_y_off, glyph_y = glyph_y_off;
       pen_y < height && glyph_y < bitmap->rows;
       ++pen_y, ++glyph_y) {
    for (unsigned pen_x = pen_x_off, glyph_x = glyph_x_off;
         pen_x < width && glyph_x < bitmap->width;
         ++pen_x, ++glyph_x) {
      unsigned c = canvas[pen_y * width + pen_x];
      c += bitmap->buffer[glyph_y * bitmap->width + glyph_x];
      c = c > 255 ? 255 : c;
      canvas[pen_y * width + pen_x] = (unsigned char) c;
    }
  }
}

int freetype_load_font(freetype_context_t* ctx, const char* path) {
  int err = 0;

  memset(ctx, 0, sizeof(freetype_context_t));

  err = FT_Init_FreeType(&ctx->library);
  if (err) {
    return -1;
  }

  err = FT_New_Face(ctx->library, path, 0, &ctx->face);
  if (err) {
    return -2;
  }

  return 0;
}

int freetype_set_pxsize(freetype_context_t* ctx, unsigned width, unsigned height) {
  int err = 0;

  err = FT_Set_Pixel_Sizes(ctx->face, width, height);
  if (err) {
    return -1;
  }

  ctx->ascender = (int) (ctx->face->size->metrics.ascender >> 6);
  ctx->descender = (int) (ctx->face->size->metrics.descender >> 6);

  return 0;
}

int freetype_render_ascii(freetype_context_t* ctx,
                          unsigned char* canvas, unsigned width, unsigned height,
                          const char* ascii_text, int len) {
  FT_Face face = ctx->face;
  FT_GlyphSlot slot = face->glyph;
  FT_Bitmap* bitmap = &slot->bitmap;
  int advance = 0;
  int x_off = 0;
  int y_off = 0;
  int err = 0;
  int cnt = 0;

  int maxAscent = (int) (face->size->metrics.ascender >> 6);

  for (int i = 0; i < len; ++i) {
    err = FT_Load_Char(ctx->face, ascii_text[i], FT_LOAD_RENDER);
    if (err) {
      continue;
    }

    advance = (int) (slot->advance.x >> 6);

    if ((x_off + advance) > width) {
      break;
    }

    y_off = maxAscent - slot->bitmap_top;

    my_draw_bitmap(canvas, width, height, x_off, y_off, bitmap);

    x_off += advance;

    ++cnt;
  }

  return cnt;
}

int freetype_render_unicode(freetype_context_t* ctx,
                            unsigned char* canvas, unsigned width, unsigned height,
                            const unsigned long* unicode_text, int len) {
  FT_Face face = ctx->face;
  FT_GlyphSlot slot = face->glyph;
  FT_Bitmap* bitmap = &slot->bitmap;
  int advance = 0;
  int x_off = 0;
  int y_off = 0;
  int err = 0;
  int cnt = 0;

  int maxAscent = (int) (face->size->metrics.ascender >> 6);

  for (int i = 0; i < len; ++i) {
    err = FT_Load_Char(ctx->face, unicode_text[i], FT_LOAD_RENDER);
    if (err) {
      continue;
    }

    advance = (int) (slot->advance.x >> 6);

    if ((x_off + advance) > width) {
      break;
    }

    y_off = maxAscent - slot->bitmap_top;

    my_draw_bitmap(canvas, width, height, x_off, y_off, bitmap);

    x_off += advance;

    ++cnt;
  }

  return cnt;
}

void freetype_free(freetype_context_t* ctx) {
  if (ctx) {
    if (ctx->face) {
      FT_Done_Face(ctx->face);
    }
    if (ctx->library) {
      FT_Done_FreeType(ctx->library);
    }
    memset(ctx, 0, sizeof(freetype_context_t));
  }
}
