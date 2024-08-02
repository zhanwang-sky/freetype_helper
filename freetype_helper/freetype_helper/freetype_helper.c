//
//  freetype_helper.c
//  freetype_helper
//
//  Created by zhanwang-sky on 2024/8/2.
//

#include <limits.h>
#include <string.h>
#include "freetype_helper.h"

static void my_draw_bitmap(unsigned char* canvas, int width, int height,
                           int x_off, int y_off, FT_Bitmap* bitmap) {
  int pen_x_off = (x_off < 0) ? 0 : x_off;
  int pen_y_off = (y_off < 0) ? 0 : y_off;
  unsigned int glyph_x_off = (x_off < 0) ? -x_off : 0;
  unsigned int glyph_y_off = (y_off < 0) ? -y_off : 0;

  int pen_x, pen_y;
  unsigned int glyph_x, glyph_y;

  for (pen_y = pen_y_off, glyph_y = glyph_y_off;
       pen_y < height && glyph_y < bitmap->rows;
       ++pen_y, ++glyph_y) {
    for (pen_x = pen_x_off, glyph_x = glyph_x_off;
         pen_x < width && glyph_x < bitmap->width;
         ++pen_x, ++glyph_x) {
      unsigned int c = canvas[pen_y * width + pen_x];
      c += bitmap->buffer[glyph_y * bitmap->width + glyph_x];
      c = (c > 255) ? 255 : c;
      canvas[pen_y * width + pen_x] = (unsigned char) c;
    }
  }
}

int freetype_load_font(freetype_context_t* ctx, const char* path) {
  int rc = 0;

  // sanity check
  if (!ctx || !path) {
    return INT_MIN;
  }

  memset(ctx, 0, sizeof(freetype_context_t));

  rc = FT_Init_FreeType(&ctx->library);
  if (rc != 0) {
    return -1;
  }

  rc = FT_New_Face(ctx->library, path, 0, &ctx->face);
  if (rc != 0) {
    return -2;
  }

  return 0;
}

int freetype_set_pxsize(freetype_context_t* ctx, int width, int height) {
  int rc = 0;

  // sanity check
  if (!ctx || !ctx->library || !ctx->face ||
      width < 0 || height < 0 || (!width && !height)) {
    return INT_MIN;
  }

  rc = FT_Set_Pixel_Sizes(ctx->face, width, height);
  if (rc != 0) {
    return -1;
  }

  ctx->ascender = (int) (ctx->face->size->metrics.ascender >> 6);
  ctx->descender = (int) (ctx->face->size->metrics.descender >> 6);

  return 0;
}

int freetype_render_ascii(freetype_context_t* ctx,
                          unsigned char* canvas, int width, int height,
                          const char* ascii_text, int len) {
  FT_Face face = NULL;
  FT_GlyphSlot slot = NULL;
  FT_Bitmap* bitmap = NULL;

  int advance = 0;
  int x_off = 0;
  int y_off = 0;

  int rc = 0;
  int cnt = 0;

  // sanity check
  if (!ctx || !ctx->library || !ctx->face ||
      !canvas || width <= 0 || height <= 0 ||
      !ascii_text || len <= 0) {
    return INT_MIN;
  }

  face = ctx->face;
  slot = face->glyph;
  bitmap = &slot->bitmap;

  for (int i = 0; i < len; ++i) {
    rc = FT_Load_Char(face, ascii_text[i], FT_LOAD_RENDER);
    if (rc != 0) {
      continue;
    }

    advance = (int) (slot->advance.x >> 6);

    if ((x_off + advance) > width) {
      break;
    }

    y_off = ctx->ascender - slot->bitmap_top;

    my_draw_bitmap(canvas, width, height, x_off, y_off, bitmap);

    x_off += advance;

    ++cnt;
  }

  return cnt;
}

int freetype_render_unicode(freetype_context_t* ctx,
                            unsigned char* canvas, int width, int height,
                            const unsigned long* unicode_text, int len) {
  FT_Face face = NULL;
  FT_GlyphSlot slot = NULL;
  FT_Bitmap* bitmap = NULL;

  int advance = 0;
  int x_off = 0;
  int y_off = 0;

  int rc = 0;
  int cnt = 0;

  // sanity check
  if (!ctx || !ctx->library || !ctx->face ||
      !canvas || width <= 0 || height <= 0 ||
      !unicode_text || len <= 0) {
    return INT_MIN;
  }

  face = ctx->face;
  slot = face->glyph;
  bitmap = &slot->bitmap;

  for (int i = 0; i < len; ++i) {
    rc = FT_Load_Char(face, unicode_text[i], FT_LOAD_RENDER);
    if (rc != 0) {
      continue;
    }

    advance = (int) (slot->advance.x >> 6);

    if ((x_off + advance) > width) {
      break;
    }

    y_off = ctx->ascender - slot->bitmap_top;

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
      ctx->face = NULL;
    }
    if (ctx->library) {
      FT_Done_FreeType(ctx->library);
      ctx->library = NULL;
    }
    ctx->ascender = 0;
    ctx->descender = 0;
  }
}
