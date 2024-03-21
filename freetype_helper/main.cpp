//
//  main.cpp
//  freetype_helper
//
//  Created by zhanwang-sky on 2024/3/21.
//

#include <fstream>
#include <iostream>
#include "freetype_helper/freetype_helper.h"

using std::cout;
using std::cerr;
using std::endl;

constexpr unsigned can_width = 1024;
constexpr unsigned can_height = 90;
unsigned char canvas[can_width * can_height];

int main(int argc, const char* argv[]) {
  freetype_context_t ctx;
  int rc = 0;

  if (argc != 3) {
    cerr << "Usage: ./freetype_helper <font_path> <ppm_path>\n";
    exit(EXIT_FAILURE);
  }

  const char* font_path = argv[1];
  const char* ppm_path = argv[2];

  rc = freetype_load_font(&ctx, font_path);
  if (rc < 0) {
    cerr << "Fail to load font, rc=" << rc << endl;
    exit(EXIT_FAILURE);
  }

  rc = freetype_set_pxsize(&ctx, 64, 0);
  if (rc < 0) {
    cerr << "Fail to set font size, rc=" << rc << endl;
    exit(EXIT_FAILURE);
  }

  cout << "ascender=" << ctx.ascender << ", descender=" << ctx.descender << endl;

  std::wstring wstr = L"摩根大通集团（J.P.Morgan Chase & Co, NYSE：JPM）";
  std::vector<unsigned long> unicode;

  for (const auto& wc : wstr) {
    unicode.push_back(static_cast<unsigned long>(wc));
  }

  rc = freetype_render_unicode(&ctx, canvas, can_width, can_height,
                               unicode.data(), (int) unicode.size());
  if (rc < 0) {
    cerr << "Fail to render unicode text, rc=" << rc << endl;
    exit(EXIT_FAILURE);
  }

  cout << rc << " characters rendered (total: " << wstr.length() << ")\n";

  std::ofstream ofs(ppm_path, std::ofstream::trunc | std::ofstream::binary);
  if (!ofs) {
    cerr << "Fail to open output file\n";
    exit(EXIT_FAILURE);
  }

  ofs << "P5\n";
  ofs << can_width << endl;
  ofs << can_height << endl;
  ofs << "255\n";
  ofs.write((const char*) canvas, can_width * can_height);

  freetype_free(&ctx);

  return 0;
}
