//
//  main.cpp
//  freetype_helper
//
//  Created by zhanwang-sky on 2024/8/2.
//

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "freetype_helper.h"

using std::cout;
using std::cerr;
using std::endl;

const int px_size = 28;
const int canv_width = 1024;
const int canv_height = 80;
unsigned char canvas[canv_width * canv_height];

int main(int argc, char* argv[]) {
  freetype_context_t ctx = {0};
  std::wstring text = L"摩根大通集团（J.P.Morgan Chase & Co, NYSE：JPM）";
  std::vector<unsigned long> unicode_text;
  int rc = 0;

  if (argc != 3) {
    cerr << "Usage: ./freetype_helper <font_path> <output.ppm>\n";
    exit(EXIT_FAILURE);
  }

  rc = freetype_load_font(&ctx, argv[1]);
  if (rc != 0) {
    cerr << "fail to load font, rc=" << rc << endl;
    exit(EXIT_FAILURE);
  }

  rc = freetype_set_pxsize(&ctx, px_size, 0);
  if (rc != 0) {
    cerr << "fail to set font size, rc=" << rc << endl;
    exit(EXIT_FAILURE);
  }

  cout << "pxsize=" << px_size
       << ", ascender=" << ctx.ascender
       << ", descender=" << ctx.descender
       << endl;

  for (const auto& wc : text) {
    unicode_text.push_back(wc);
  }

  rc = freetype_render_unicode(&ctx, canvas, canv_width, canv_height,
                               unicode_text.data(),
                               static_cast<int>(unicode_text.size()));
  if (rc < 0) {
    cerr << "fail to render text, rc=" << rc << endl;
    exit(EXIT_FAILURE);
  }

  cout << rc << " characters rendered\n";

  std::ofstream ofs(argv[2], std::fstream::binary | std::fstream::trunc);
  if (!ofs) {
    cerr << "fail to open ppm file for writing\n";
    exit(EXIT_FAILURE);
  }

  ofs << "P5\n" << canv_width << " " << canv_height << "\n255\n";
  ofs.write(reinterpret_cast<const char*>(canvas), canv_width * canv_height);

  return 0;
}
